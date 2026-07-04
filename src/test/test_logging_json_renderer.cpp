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

#include "test_logging_json_renderer.h"

#include "common/logging.h"
#include "common/logging_context.h"

#include <cctype>
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

namespace
{

enum class TestLogKind : std::uint8_t
{
    Warrior = 1,
};

auto render(const logging::detail::LogRecord& rec) -> std::string
{
    fmt::memory_buffer buf;
    logging::detail::renderJsonLine(buf, rec);
    return std::string(buf.data(), buf.size());
}

auto timestampHasExpectedShape(std::string_view ts) -> bool
{
    if (ts.size() != 24)
    {
        return false;
    }

    const auto expected = std::string_view("0000-00-00T00:00:00.000Z");
    for (std::size_t i = 0; i < ts.size(); ++i)
    {
        if (expected[i] == '0')
        {
            if (!std::isdigit(static_cast<unsigned char>(ts[i])))
            {
                return false;
            }
        }
        else if (ts[i] != expected[i])
        {
            return false;
        }
    }

    return true;
}

auto normalizeTimestamp(std::string line) -> std::optional<std::string>
{
    constexpr auto prefix = std::string_view(R"({"ts":")");
    if (line.rfind(prefix, 0) != 0)
    {
        return std::nullopt;
    }

    const auto end = line.find('"', prefix.size());
    if (end == std::string::npos)
    {
        return std::nullopt;
    }

    const auto timestamp = std::string_view(line).substr(prefix.size(), end - prefix.size());
    if (!timestampHasExpectedShape(timestamp))
    {
        return std::nullopt;
    }

    line.replace(prefix.size(), timestamp.size(), "<ts>");
    return line;
}

auto expectNormalizedLine(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    const auto normalized = normalizeTimestamp(actual);
    if (!normalized || *normalized != expected)
    {
        std::cerr << "logging json renderer self-test failed: " << label << '\n';
        std::cerr << "actual:   " << actual << '\n';
        if (normalized)
        {
            std::cerr << "normalized: " << *normalized << '\n';
        }
        std::cerr << "expected: " << expected << '\n';
        return false;
    }

    return true;
}

auto testRecordAndContextRendering() -> bool
{
    const auto message = std::string("quote\" slash\\ back\b form\f line\n return\r tab\t ctrl") + static_cast<char>(0x01);

    logging::LogScope scope{
        { "char_id", std::uint64_t{ 12345 } },
        { "zone", std::int64_t{ -7 } },
        { "ratio", 1.25 },
        { "active", true },
        { "job", TestLogKind::Warrior },
        { "unknown_job", static_cast<TestLogKind>(99) },
        { "key\"quote", std::string("value\\text") },
        { "nested", { { "name", "Bastok" }, { "ok", false } } },
    };

    const auto actual = render({ "warn\"level", R"(C:\xi\server.cpp)", 42, "fn\\name", message });
    const auto expected = std::string(R"({"ts":"<ts>","lvl":"warn\"level","source":{"file":"server.cpp","line":42,"function":"fn\\name"},"msg":"quote\" slash\\ back\b form\f line\n return\r tab\t ctrl\u0001","char_id":12345,"zone":-7,"ratio":1.25,"active":true,"job":"Warrior","unknown_job":99,"key\"quote":"value\\text","nested":{"name":"Bastok","ok":false}})");

    return expectNormalizedLine(actual, expected, "record and scoped context");
}

auto testRecordWithoutContextRendering() -> bool
{
    const auto actual   = render({ "info", "/opt/xi/map.cpp", 7, "tick", "ready" });
    const auto expected = std::string(R"({"ts":"<ts>","lvl":"info","source":{"file":"map.cpp","line":7,"function":"tick"},"msg":"ready"})");

    return expectNormalizedLine(actual, expected, "record without context");
}

auto testHighByteStringRendering() -> bool
{
    const auto actual   = render({ "debug", "map.cpp", 0, "fn", std::string("h") + static_cast<char>(0x80) + "i" });
    const auto expected = std::string(R"({"ts":"<ts>","lvl":"debug","source":{"file":"map.cpp","line":0,"function":"fn"},"msg":"h)") + static_cast<char>(0x80) + R"(i"})";

    return expectNormalizedLine(actual, expected, "high byte string");
}

} // namespace

auto runLoggingJsonRendererSelfTests() -> bool
{
    bool ok = true;

    ok = testRecordAndContextRendering() && ok;
    ok = testRecordWithoutContextRendering() && ok;
    ok = testHighByteStringRendering() && ok;

    return ok;
}
