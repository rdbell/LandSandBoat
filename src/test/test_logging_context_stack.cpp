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

#include "test_logging_context_stack.h"

#include "common/logging_context.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>

namespace
{

auto expect(bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "logging context stack self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectSize(std::size_t actual, std::size_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "logging context stack self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

template <typename T>
auto expectFieldValue(const logging::Field& field, std::string_view key, const T& expected, const std::string& label) -> bool
{
    bool ok = true;
    ok      = expect(field.key == key, label + " key") && ok;
    ok      = expect(std::holds_alternative<T>(field.value), label + " type") && ok;
    if (std::holds_alternative<T>(field.value))
    {
        ok = expect(std::get<T>(field.value) == expected, label + " value") && ok;
    }
    return ok;
}

auto testNestedScopeStack() -> bool
{
    bool ok = true;
    ok      = expect(logging::currentContext().empty(), "initial context empty") && ok;

    {
        logging::LogScope outer{
            { "char_id", std::uint64_t{ 42 } },
            { "name", std::string("Omega") },
        };

        const auto& outerContext = logging::currentContext();
        ok                       = expectSize(outerContext.size(), 2, "outer context size") && ok;
        if (outerContext.size() == 2)
        {
            ok = expectFieldValue<std::uint64_t>(outerContext[0], "char_id", 42, "outer char id") && ok;
            ok = expectFieldValue<std::string>(outerContext[1], "name", "Omega", "outer name") && ok;
        }

        {
            logging::LogScope inner{
                { "zone", std::int64_t{ -3 } },
                { "active", true },
                { "nested", { { "inner", "value" } } },
            };

            const auto& innerContext = logging::currentContext();
            ok                       = expectSize(innerContext.size(), 5, "inner context size") && ok;
            if (innerContext.size() == 5)
            {
                ok = expectFieldValue<std::uint64_t>(innerContext[0], "char_id", 42, "inner char id") && ok;
                ok = expectFieldValue<std::string>(innerContext[1], "name", "Omega", "inner name") && ok;
                ok = expectFieldValue<std::int64_t>(innerContext[2], "zone", -3, "inner zone") && ok;
                ok = expectFieldValue<bool>(innerContext[3], "active", true, "inner active") && ok;
                ok = expect(innerContext[4].key == "nested", "nested key") && ok;
                ok = expect(std::holds_alternative<logging::Subfields>(innerContext[4].value), "nested type") && ok;
                if (std::holds_alternative<logging::Subfields>(innerContext[4].value))
                {
                    const auto& nested = std::get<logging::Subfields>(innerContext[4].value);
                    ok                 = expectSize(nested.size(), 1, "nested size") && ok;
                    if (nested.size() == 1)
                    {
                        ok = expectFieldValue<std::string_view>(nested[0], "inner", std::string_view("value"), "nested inner") && ok;
                    }
                }
            }
        }

        const auto& afterInner = logging::currentContext();
        ok                    = expectSize(afterInner.size(), 2, "after inner context size") && ok;
        if (afterInner.size() == 2)
        {
            ok = expectFieldValue<std::uint64_t>(afterInner[0], "char_id", 42, "after inner char id") && ok;
            ok = expectFieldValue<std::string>(afterInner[1], "name", "Omega", "after inner name") && ok;
        }
    }

    ok = expect(logging::currentContext().empty(), "final context empty") && ok;

    return ok;
}

} // namespace
auto runLoggingContextStackSelfTests() -> bool
{
    return testNestedScopeStack();
}
