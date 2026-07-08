/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_settings_loader.h"

#include "common/logging.h"
#include "common/lua.h"
#include "common/settings.h"

#include <atomic>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace
{

class SettingsStateGuard
{
public:
    SettingsStateGuard()
    : map_(settings::settingsMap)
    , generation_(settings::detail::generation.load(std::memory_order_acquire))
    , luaSettings_(lua["xi"].get_or_create<sol::table>()["settings"])
    , loggingPattern_(settings::get<std::string>("logging.PATTERN"))
    {
        settings::settingsMap.clear();
        settings::detail::generation.fetch_add(1, std::memory_order_release);
        lua["xi"]["settings"] = lua.create_table();
    }

    ~SettingsStateGuard()
    {
        settings::settingsMap = map_;
        lua["xi"]["settings"] = luaSettings_;
        settings::detail::generation.store(generation_ + 1, std::memory_order_release);
        logging::SetPattern(loggingPattern_);
    }

private:
    settings::SettingsMap map_;
    std::uint64_t         generation_;
    sol::object           luaSettings_;
    std::string           loggingPattern_;
};

class TempSettingsRoot
{
public:
    TempSettingsRoot()
    : originalPath_(std::filesystem::current_path())
    , tempPath_(std::filesystem::temp_directory_path() / ("lsb_settings_loader_" + std::to_string(processID()) + "_" + std::to_string(nextID())))
    {
        std::filesystem::remove_all(tempPath_);
        std::filesystem::create_directories(tempPath_ / "settings/default");
        std::filesystem::create_directories(tempPath_ / "settings");
        write("settings/default/logging.lua", R"(
xi = xi or {}
xi.settings = xi.settings or {}
xi.settings.logging = xi.settings.logging or {}
xi.settings.logging.PATTERN = "%v"
xi.settings.logging.JSON_ENABLED = false
)");
        std::filesystem::current_path(tempPath_);
    }

    ~TempSettingsRoot()
    {
        std::filesystem::current_path(originalPath_);
        std::filesystem::remove_all(tempPath_);
    }

    void write(const std::filesystem::path& relativePath, std::string_view content) const
    {
        const auto path = tempPath_ / relativePath;
        std::filesystem::create_directories(path.parent_path());
        std::ofstream output(path);
        output << content;
    }

private:
    static auto nextID() -> std::uint64_t
    {
        static std::atomic_uint64_t counter{ 0 };
        return counter.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    static auto processID() -> std::uint64_t
    {
#ifdef _WIN32
        return static_cast<std::uint64_t>(GetCurrentProcessId());
#else
        return static_cast<std::uint64_t>(getpid());
#endif
    }

    std::filesystem::path originalPath_;
    std::filesystem::path tempPath_;
};

class EnvGuard
{
public:
    explicit EnvGuard(std::string key)
    : key_(std::move(key))
    {
        if (const auto* value = std::getenv(key_.c_str()))
        {
            original_ = value;
        }
    }

    ~EnvGuard()
    {
        if (!changed_)
        {
            return;
        }

        if (original_)
        {
            set(*original_);
        }
        else
        {
#ifdef _WIN32
            _putenv_s(key_.c_str(), "");
#else
            unsetenv(key_.c_str());
#endif
        }
    }

    void set(std::string_view value)
    {
#ifdef _WIN32
        _putenv_s(key_.c_str(), std::string(value).c_str());
#else
        setenv(key_.c_str(), std::string(value).c_str(), 1);
#endif
        changed_ = true;
    }

private:
    std::string               key_;
    bool                      changed_{ false };
    std::optional<std::string> original_;
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "settings loader self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectThrows(const auto& action, const std::string& label) -> bool
{
    try
    {
        action();
    }
    catch (...)
    {
        return true;
    }

    std::cerr << "settings loader self-test failed: expected throw in " << label << '\n';
    return false;
}

auto testDefaultsUserAndCaseSensitivity() -> bool
{
    SettingsStateGuard state;
    TempSettingsRoot   root;

    root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = xi.settings or {}
xi.settings.main = xi.settings.main or {}
xi.settings.main.BASE = 10
xi.settings.main.KEPT = 12
xi.settings.main.CaseSensitive = 99
)");
    root.write("settings/main.lua", R"(
xi.settings.main.BASE = 20
)");

    settings::init();

    bool ok = true;
    ok      = expectEqual(settings::get<int>("main.BASE"), 20, "user overrides default") && ok;
    ok      = expectEqual(settings::get<int>("main.KEPT"), 12, "default remains") && ok;
    ok      = expectEqual(settings::get<int>("main.CaseSensitive"), 99, "exact case key") && ok;
    ok      = expectEqual(settings::get<int>("main.Casesensitive"), 0, "case-mismatched key missing") && ok;
    return ok;
}

auto testEnvironmentOverrides() -> bool
{
    SettingsStateGuard state;
    TempSettingsRoot   root;
    EnvGuard           valueEnv("XI_MAIN_VALUE");
    EnvGuard           keptEnv("XI_MAIN_KEPT");
    EnvGuard           replacedEnv("XI_MAIN_REPLACED");

    root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = xi.settings or {}
xi.settings.main = {
    VALUE = 10,
    KEPT = 1,
    REPLACED = 2,
}
)");
    root.write("settings/main.lua", R"(
xi.settings.main = {
    VALUE = 20,
    REPLACED = 3,
    TABLE_VALUE = { nested = true },
}
)");

    valueEnv.set("00123");
    keptEnv.set("99");
    replacedEnv.set("42");
    EnvGuard tableEnv("XI_MAIN_TABLE_VALUE");
    tableEnv.set("table-env");
    settings::init();

    bool ok = true;
    ok      = expectEqual(settings::get<double>("main.VALUE"), 123.0, "numeric env override") && ok;
    ok      = expectEqual(settings::get<int>("main.KEPT"), 1, "env ignores default key missing from current Lua table") && ok;
    ok      = expectEqual(settings::get<int>("main.REPLACED"), 42, "env applies to current user key") && ok;
    ok      = expectEqual(settings::get<std::string>("main.TABLE_VALUE"), std::string("table-env"), "env applies to current non-primitive key") && ok;

    valueEnv.set("12.3");
    settings::init();
    ok = expectEqual(settings::get<std::string>("main.VALUE"), std::string("12.3"), "non-number env remains string") && ok;

    valueEnv.set("   ");
    ok = expectThrows([]() { settings::init(); }, "all-space numeric env override") && ok;
    return ok;
}

auto testMalformedSettingsShape() -> bool
{
    bool ok = true;

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = 1
)");
        ok = expectThrows([]() { settings::init(); }, "scalar xi.settings") && ok;
    }

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = {}
xi.settings[1] = { VALUE = 1 }
)");
        ok = expectThrows([]() { settings::init(); }, "numeric outer settings key") && ok;
    }

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = {}
xi.settings.main = 1
)");
        ok = expectThrows([]() { settings::init(); }, "scalar outer settings value") && ok;
    }

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/default/main.lua", R"(
xi = xi or {}
xi.settings = {}
xi.settings.main = {}
xi.settings.main[1] = 2
)");
        ok = expectThrows([]() { settings::init(); }, "numeric inner settings key") && ok;
    }

    return ok;
}

auto testMissingVisitSetAndParseErrors() -> bool
{
    bool ok = true;

    {
        SettingsStateGuard state;
        ok = expectEqual(settings::get<int>("main.MISSING"), 0, "missing int") && ok;
        ok = expectEqual(settings::get<std::string>("main.MISSING"), std::string(""), "missing string") && ok;
        ok = expectEqual(settings::get<bool>("main.MISSING"), false, "missing bool") && ok;

        settings::set("cache.value", 1.0);
        const auto before = settings::get<int>("cache.value");
        settings::set("cache.value", 2.0);
        const auto after = settings::get<int>("cache.value");

        std::unordered_map<std::string, settings::SettingsVariant> seen;
        settings::visit([&](const std::string& key, const settings::SettingsVariant& value) { seen[key] = value; });

        ok = expectEqual(before, 1, "cached read before set") && ok;
        ok = expectEqual(after, 2, "set invalidates cached read") && ok;
        ok = expectEqual(seen.find("cache.value") != seen.end(), true, "visit includes set key") && ok;
    }

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/default/main.lua", "this is not valid lua\n");
        ok = expectThrows([]() { settings::init(); }, "invalid default settings script") && ok;
    }

    {
        SettingsStateGuard state;
        TempSettingsRoot   root;
        root.write("settings/main.lua", "this is not valid lua\n");
        ok = expectThrows([]() { settings::init(); }, "invalid user settings script") && ok;
    }

    return ok;
}

} // namespace

auto runSettingsLoaderSelfTests() -> bool
{
    bool ok = true;
    ok      = testDefaultsUserAndCaseSensitivity() && ok;
    ok      = testEnvironmentOverrides() && ok;
    ok      = testMalformedSettingsShape() && ok;
    ok      = testMissingVisitSetAndParseErrors() && ok;
    return ok;
}
