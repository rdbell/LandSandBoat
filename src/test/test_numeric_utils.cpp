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

#include "test_numeric_utils.h"

#include "common/utils.h"

#include <array>
#include <cmath>
#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "numeric utils self-test failed: " << label << " expected " << expected << ", got " << actual << '\n';
        return false;
    }

    return true;
}

auto expectMSB(uint32 value, uint32 expected, const std::string& label) -> bool
{
    uint32 actual = 123;
    getMSB(&actual, value);
    return expectEqual(actual, expected, label);
}

} // namespace

auto runNumericUtilsSelfTests() -> bool
{
    bool ok = true;

    ok = expectEqual(roundUpToNearestFour(0U), 0U, "roundUpToNearestFour zero") && ok;
    ok = expectEqual(roundUpToNearestFour(1U), 4U, "roundUpToNearestFour one") && ok;
    ok = expectEqual(roundUpToNearestFour(4U), 4U, "roundUpToNearestFour exact") && ok;
    ok = expectEqual(roundUpToNearestFour(5U), 8U, "roundUpToNearestFour five") && ok;
    ok = expectEqual(roundUpToNearestFour(15U), 16U, "roundUpToNearestFour fifteen") && ok;
    ok = expectEqual(roundUpToNearestFour(0xFFFFFFFCU), 0xFFFFFFFCU, "roundUpToNearestFour max exact") && ok;
    ok = expectEqual(roundUpToNearestFour(0xFFFFFFFFU), 0U, "roundUpToNearestFour wrap") && ok;

    ok = expectEqual(intpow32(2, 0), 1, "intpow32 zero exponent") && ok;
    ok = expectEqual(intpow32(2, 10), 1024, "intpow32 power of two") && ok;
    ok = expectEqual(intpow32(-2, 5), -32, "intpow32 negative odd") && ok;
    ok = expectEqual(intpow32(-2, 6), 64, "intpow32 negative even") && ok;
    ok = expectEqual(intpow32(3, 7), 2187, "intpow32 power of three") && ok;
    ok = expectEqual(intpow32(5, 1), 5, "intpow32 exponent one") && ok;
    ok = expectEqual(intpow32(0, 0), 1, "intpow32 zero to zero") && ok;

    ok = expectMSB(0U, 0U, "getMSB zero") && ok;
    ok = expectMSB(1U, 0U, "getMSB one") && ok;
    ok = expectMSB(2U, 1U, "getMSB two") && ok;
    ok = expectMSB(3U, 1U, "getMSB three") && ok;
    ok = expectMSB(4U, 2U, "getMSB four") && ok;
    ok = expectMSB(0x00F00000U, 23U, "getMSB middle") && ok;
    ok = expectMSB(0x80000000U, 31U, "getMSB high bit") && ok;
    ok = expectMSB(0xFFFFFFFFU, 31U, "getMSB all bits") && ok;

    const float one        = 1.0f;
    const float oneUp      = std::nextafter(one, 2.0f);
    const float oneUpTwice = std::nextafter(oneUp, 2.0f);

    ok = expectEqual(approximatelyEqual(one, one), true, "approximatelyEqual equal") && ok;
    ok = expectEqual(essentiallyEqual(one, one), true, "essentiallyEqual equal") && ok;
    ok = expectEqual(definitelyGreaterThan(one, one), false, "definitelyGreaterThan equal") && ok;
    ok = expectEqual(definitelyLessThan(one, one), false, "definitelyLessThan equal") && ok;

    ok = expectEqual(approximatelyEqual(one, oneUp), true, "approximatelyEqual one ulp") && ok;
    ok = expectEqual(essentiallyEqual(one, oneUp), true, "essentiallyEqual one ulp") && ok;
    ok = expectEqual(definitelyGreaterThan(oneUp, one), false, "definitelyGreaterThan one ulp") && ok;
    ok = expectEqual(definitelyLessThan(one, oneUp), false, "definitelyLessThan one ulp") && ok;

    ok = expectEqual(approximatelyEqual(one, oneUpTwice), false, "approximatelyEqual two ulps") && ok;
    ok = expectEqual(essentiallyEqual(one, oneUpTwice), false, "essentiallyEqual two ulps") && ok;
    ok = expectEqual(definitelyGreaterThan(oneUpTwice, one), true, "definitelyGreaterThan two ulps") && ok;
    ok = expectEqual(definitelyLessThan(one, oneUpTwice), true, "definitelyLessThan two ulps") && ok;

    return ok;
}
