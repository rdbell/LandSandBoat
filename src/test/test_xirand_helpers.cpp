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

#include "test_xirand_helpers.h"

#include "common/xirand.h"

#include <cstddef>
#include <iostream>
#include <map>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace
{

struct NoDefault
{
    explicit NoDefault(int value)
    : value(value)
    {
    }

    int value;
};

auto operator<(const NoDefault& lhs, const NoDefault& rhs) -> bool
{
    return lhs.value < rhs.value;
}

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "xirand helpers self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectTrue(bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "xirand helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename Fn>
auto expectOutOfRange(Fn&& fn, const std::string& label) -> bool
{
    try
    {
        fn();
    }
    catch (const std::out_of_range&)
    {
        return true;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "xirand helpers self-test failed: " << label << " threw wrong exception " << ex.what() << '\n';
        return false;
    }

    std::cerr << "xirand helpers self-test failed: " << label << " did not throw" << '\n';
    return false;
}

auto testGuardedNumbers() -> bool
{
    bool ok = true;

    ok = expectEqual(xirand::GetRandomNumber<int>(5, 5), 5, "integer equal bounds") && ok;
    ok = expectEqual(xirand::GetRandomNumber<int>(8, 3), 8, "integer reversed bounds") && ok;
    ok = expectEqual(xirand::GetRandomNumber<int>(0), 0, "integer zero max") && ok;
    ok = expectEqual(xirand::GetRandomNumber<double>(2.5, 2.5), 2.5, "float equal bounds") && ok;
    ok = expectEqual(xirand::GetRandomNumber<double>(3.5, -1.0), 3.5, "float reversed bounds") && ok;

    return ok;
}

auto testNonEmptyDomains() -> bool
{
    bool ok = true;

    const auto integer = xirand::GetRandomNumber<int>(10, 20);
    ok                 = expectTrue(integer >= 10 && integer < 20, "integer in half-open range") && ok;

    const auto floating = xirand::GetRandomNumber<double>(1.5, 2.5);
    ok                  = expectTrue(floating >= 1.5 && floating < 2.5, "float in half-open range") && ok;

    const auto element = xirand::GetRandomElement(std::vector<std::string>{ "alpha", "bravo", "charlie" });
    ok                 = expectTrue(element == "alpha" || element == "bravo" || element == "charlie", "random element from input") && ok;

    const auto weightedIndex = xirand::GetWeightedIndex({ 1.0, 3.0, 6.0 });
    ok                       = expectTrue(weightedIndex < 3, "weighted index in range") && ok;

    const auto weightedElement = xirand::GetWeightedElement(std::map<std::string, double>{ { "Common", 70.0 }, { "Rare", 30.0 } });
    ok                         = expectTrue(weightedElement == "Common" || weightedElement == "Rare", "weighted element from input") && ok;

    const auto customWeightedElement = xirand::GetWeightedElement(std::map<NoDefault, double>{ { NoDefault(1), 1.0 }, { NoDefault(2), 1.0 } });
    ok                               = expectTrue(customWeightedElement.value == 1 || customWeightedElement.value == 2, "custom weighted element from input") && ok;

    return ok;
}

auto testEmptyRandomElement() -> bool
{
    bool ok = true;

    ok = expectEqual(xirand::GetRandomElement(std::vector<int>{}), 0, "empty int element") && ok;
    ok = expectEqual(xirand::GetRandomElement(std::vector<std::string>{}), std::string{}, "empty string element") && ok;
    ok = expectOutOfRange(
             []()
             {
                 std::ignore = xirand::GetRandomElement(std::vector<NoDefault>{});
             },
             "empty non-default element") &&
         ok;

    return ok;
}

auto testEmptyWeightedHelpers() -> bool
{
    bool ok = true;

    ok = expectEqual(xirand::GetWeightedIndex(std::span<const double>{}), static_cast<std::size_t>(0), "empty span weighted index") && ok;
    ok = expectEqual(xirand::GetWeightedIndex(std::initializer_list<double>{}), static_cast<std::size_t>(0), "empty list weighted index") && ok;
    ok = expectEqual(xirand::GetWeightedElement(std::map<std::string, double>{}), std::string{}, "empty defaultable weighted element") && ok;
    ok = expectOutOfRange(
             []()
             {
                 std::ignore = xirand::GetWeightedElement(std::map<NoDefault, double>{});
             },
             "empty non-default weighted element") &&
         ok;

    return ok;
}

} // namespace

auto runXirandHelpersSelfTests() -> bool
{
    bool ok = true;

    ok = testGuardedNumbers() && ok;
    ok = testNonEmptyDomains() && ok;
    ok = testEmptyRandomElement() && ok;
    ok = testEmptyWeightedHelpers() && ok;

    return ok;
}
