/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_settings_value_conversions.h"

#include "common/settings.h"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace
{

class SettingGuard
{
public:
    explicit SettingGuard(std::string key)
    : key_(std::move(key))
    {
        if (const auto it = settings::settingsMap.find(key_); it != settings::settingsMap.end())
        {
            original_ = it->second;
        }
    }

    ~SettingGuard()
    {
        if (original_)
        {
            settings::settingsMap[key_] = *original_;
        }
        else
        {
            settings::settingsMap.erase(key_);
        }

        settings::detail::generation.fetch_add(1, std::memory_order_release);
    }

private:
    std::string                              key_;
    std::optional<settings::SettingsVariant> original_;
};

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "settings value conversion self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testBoolConversions(const std::string& key) -> bool
{
    bool ok = true;

    settings::set(key, true);
    ok = expectEqual(settings::getUncached<bool>(key), true, "bool true to bool") && ok;
    ok = expectEqual(settings::getUncached<double>(key), 1.0, "bool true to double") && ok;
    ok = expectEqual(settings::getUncached<std::int16_t>(key), static_cast<std::int16_t>(1), "bool true to signed") && ok;
    ok = expectEqual(settings::getUncached<std::uint8_t>(key), static_cast<std::uint8_t>(1), "bool true to unsigned") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("true"), "bool true to string") && ok;

    settings::set(key, false);
    ok = expectEqual(settings::getUncached<bool>(key), false, "bool false to bool") && ok;
    ok = expectEqual(settings::getUncached<double>(key), 0.0, "bool false to double") && ok;
    ok = expectEqual(settings::getUncached<std::int16_t>(key), static_cast<std::int16_t>(0), "bool false to signed") && ok;
    ok = expectEqual(settings::getUncached<std::uint8_t>(key), static_cast<std::uint8_t>(0), "bool false to unsigned") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("false"), "bool false to string") && ok;

    return ok;
}

auto testDoubleConversions(const std::string& key) -> bool
{
    bool ok = true;

    settings::set(key, 12.75);
    ok = expectEqual(settings::getUncached<bool>(key), true, "double nonzero to bool") && ok;
    ok = expectEqual(settings::getUncached<float>(key), 12.75F, "double to float") && ok;
    ok = expectEqual(settings::getUncached<double>(key), 12.75, "double to double") && ok;
    ok = expectEqual(settings::getUncached<std::int16_t>(key), static_cast<std::int16_t>(12), "double to signed truncates") && ok;
    ok = expectEqual(settings::getUncached<std::uint8_t>(key), static_cast<std::uint8_t>(12), "double to unsigned truncates") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("12.75"), "double to string") && ok;

    settings::set(key, 0.0);
    ok = expectEqual(settings::getUncached<bool>(key), false, "double zero to bool") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("0"), "double zero to string") && ok;

    settings::set(key, -3.5);
    ok = expectEqual(settings::getUncached<bool>(key), true, "negative double to bool") && ok;
    ok = expectEqual(settings::getUncached<std::int16_t>(key), static_cast<std::int16_t>(-3), "negative double to signed") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("-3.5"), "negative double to string") && ok;

    settings::set(key, 257.0);
    ok = expectEqual(settings::getUncached<std::uint8_t>(key), static_cast<std::uint8_t>(1), "double to unsigned narrows") && ok;

    settings::set(key, std::numeric_limits<double>::quiet_NaN());
    ok = expectEqual(settings::getUncached<bool>(key), true, "nan double to bool") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("nan"), "nan double to string") && ok;

    settings::set(key, std::numeric_limits<double>::infinity());
    ok = expectEqual(settings::getUncached<bool>(key), true, "infinity double to bool") && ok;
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("inf"), "infinity double to string") && ok;

    settings::set(key, -std::numeric_limits<double>::infinity());
    ok = expectEqual(settings::getUncached<std::string>(key), std::string("-inf"), "negative infinity double to string") && ok;

    return ok;
}

struct StringCase
{
    std::string value;
    bool        truthy;
    std::string label;
};

auto testStringConversions(const std::string& key) -> bool
{
    bool ok = true;

    const std::vector<StringCase> cases{
        { "", false, "empty string" },
        { "false", false, "false string" },
        { "0", false, "zero string" },
        { "False", true, "case-sensitive false string" },
        { "00", true, "double zero string" },
        { "true", true, "true string" },
        { " ", true, "space string" },
    };

    for (const auto& test : cases)
    {
        settings::set(key, test.value);

        const auto numeric = test.truthy ? 1 : 0;
        ok = expectEqual(settings::getUncached<bool>(key), test.truthy, test.label + " to bool") && ok;
        ok = expectEqual(settings::getUncached<double>(key), static_cast<double>(numeric), test.label + " to double") && ok;
        ok = expectEqual(settings::getUncached<std::int16_t>(key), static_cast<std::int16_t>(numeric), test.label + " to signed") && ok;
        ok = expectEqual(settings::getUncached<std::uint8_t>(key), static_cast<std::uint8_t>(numeric), test.label + " to unsigned") && ok;
        ok = expectEqual(settings::getUncached<std::string>(key), test.value, test.label + " to string") && ok;
    }

    return ok;
}

} // namespace

auto runSettingsValueConversionsSelfTests() -> bool
{
    const auto key = std::string("omega.SETTINGS_VALUE_CONVERSION_TEST");
    const SettingGuard guard(key);

    bool ok = true;
    ok = testBoolConversions(key) && ok;
    ok = testDoubleConversions(key) && ok;
    ok = testStringConversions(key) && ok;

    return ok;
}
