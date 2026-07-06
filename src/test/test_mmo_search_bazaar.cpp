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

#include "test_mmo_search_bazaar.h"

#include "common/mmo.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expectEqualUInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo search/bazaar self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "mmo search/bazaar self-test failed: " << label << " got length " << actual.size() << " expected length " << expected.size() << '\n';
        return false;
    }
    return true;
}

auto testSearchDefaultsAndAssignment() -> bool
{
    bool ok = true;

    search_t search{};
    ok = expectEqualUInt(search.language, 0, "search_t default language") && ok;
    ok = expectEqualUInt(search.messagetype, 0, "search_t default messagetype") && ok;
    ok = expectEqualString(search.message, "", "search_t default message") && ok;

    const std::string message{ "Need help\0Rank 5", 16 };
    search.language    = 0x1F;
    search.messagetype = 0x02;
    search.message     = message;

    ok = expectEqualUInt(search.language, 0x1F, "search_t assigned language") && ok;
    ok = expectEqualUInt(search.messagetype, 0x02, "search_t assigned messagetype") && ok;
    ok = expectEqualString(search.message, message, "search_t assigned message") && ok;
    return ok;
}

auto testBazaarDefaultsAndAssignment() -> bool
{
    bool ok = true;

    bazaar_t bazaar{};
    ok = expectEqualString(bazaar.message, "", "bazaar_t default message") && ok;

    const std::string message = "Pear au lait - half price";
    bazaar.message           = message;

    ok = expectEqualString(bazaar.message, message, "bazaar_t assigned message") && ok;
    return ok;
}

} // namespace

auto runMMOSearchBazaarSelfTests() -> bool
{
    return testSearchDefaultsAndAssignment() &&
           testBazaarDefaultsAndAssignment();
}
