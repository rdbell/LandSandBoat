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

#include "test_maybe_optional.h"

#include "common/types/maybe.h"

#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "maybe optional self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testMaybeValues() -> bool
{
    bool ok = true;

    const Maybe<int> some = 42;
    ok                    = expectEqual(some.has_value(), true, "some has value") && ok;
    ok                    = expectEqual(static_cast<bool>(some), true, "some bool") && ok;
    ok                    = expectEqual(*some, 42, "some dereference") && ok;
    ok                    = expectEqual(some.value_or(7), 42, "some value_or") && ok;

    const Maybe<int> none = std::nullopt;
    ok                    = expectEqual(none.has_value(), false, "none has value") && ok;
    ok                    = expectEqual(static_cast<bool>(none), false, "none bool") && ok;
    ok                    = expectEqual(none.value_or(7), 7, "none value_or") && ok;

    const Maybe<std::string> maybeString = std::string("Omega");
    ok                                  = expectEqual(maybeString.value_or("fallback"), std::string("Omega"), "string value") && ok;

    return ok;
}

auto testApplyTo() -> bool
{
    bool        ok       = true;
    int         called   = 0;
    std::string observed = "";

    applyTo(Maybe<std::string>{ std::string("Omega") }, [&](const std::string& value) {
        ++called;
        observed = value;
    });
    ok = expectEqual(called, 1, "applyTo some called") && ok;
    ok = expectEqual(observed, std::string("Omega"), "applyTo some observed") && ok;

    applyTo(Maybe<std::string>{ std::nullopt }, [&](const std::string& value) {
        ++called;
        observed = value;
    });
    ok = expectEqual(called, 1, "applyTo none not called") && ok;
    ok = expectEqual(observed, std::string("Omega"), "applyTo none observed unchanged") && ok;

    Maybe<int> maybeNumber = 9;
    applyTo(maybeNumber, [&](const int value) {
        called += value;
    });
    ok = expectEqual(called, 10, "applyTo int") && ok;

    return ok;
}

} // namespace

auto runMaybeOptionalSelfTests() -> bool
{
    bool ok = true;

    ok = testMaybeValues() && ok;
    ok = testApplyTo() && ok;

    return ok;
}
