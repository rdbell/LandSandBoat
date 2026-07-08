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

#include "test_trigger_area.h"

#include "trigger_area.h"

#include <iostream>

namespace
{

auto expectEqual(auto actual, auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "trigger area self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testTriggerAreaIdentityAndCount() -> bool
{
    CCuboidTriggerArea area(0, -1.0f, -2.0f, -3.0f, 4.0f, 5.0f, 6.0f);

    bool ok = true;
    ok      = expectEqual(area.getTriggerAreaID(), static_cast<uint32>(0), "zero trigger area ID retained") && ok;
    ok      = expectEqual(area.getCount(), static_cast<int16>(0), "initial count") && ok;
    ok      = expectEqual(area.addCount(3), static_cast<int16>(3), "add positive count") && ok;
    ok      = expectEqual(area.delCount(1), static_cast<int16>(2), "delete count") && ok;
    ok      = expectEqual(area.addCount(-5), static_cast<int16>(-3), "add negative count") && ok;
    return ok;
}

auto testCuboidTriggerArea() -> bool
{
    CCuboidTriggerArea area(101, -1.0f, -2.0f, -3.0f, 4.0f, 5.0f, 6.0f);

    bool ok = true;
    ok      = expectEqual(area.getTriggerAreaID(), static_cast<uint32>(101), "cuboid trigger area ID") && ok;
    ok      = expectEqual(area.isPointInside(-1.0f, -2.0f, -3.0f), true, "cuboid min boundary") && ok;
    ok      = expectEqual(area.isPointInside(4.0f, 5.0f, 6.0f), true, "cuboid max boundary") && ok;
    ok      = expectEqual(area.isPointInside(position_t{ 1.0f, 2.0f, 3.0f, 0, 0 }), true, "cuboid position overload") && ok;
    ok      = expectEqual(area.isPointInside(-1.01f, 0.0f, 0.0f), false, "cuboid x below") && ok;
    ok      = expectEqual(area.isPointInside(0.0f, 5.01f, 0.0f), false, "cuboid y above") && ok;
    ok      = expectEqual(area.isPointInside(0.0f, 0.0f, -3.01f), false, "cuboid z below") && ok;
    return ok;
}

auto testCylindricalTriggerArea() -> bool
{
    CCylindricalTriggerArea area(202, 10.0f, -5.0f, 3.0f);

    bool ok = true;
    ok      = expectEqual(area.isPointInside(10.0f, -999.0f, -5.0f), true, "cylinder center ignores y") && ok;
    ok      = expectEqual(area.isPointInside(13.0f, 999.0f, -5.0f), true, "cylinder x boundary") && ok;
    ok      = expectEqual(area.isPointInside(position_t{ 10.0f, 123.0f, -2.0f, 0, 0 }), true, "cylinder z boundary position") && ok;
    ok      = expectEqual(area.isPointInside(13.01f, 0.0f, -5.0f), false, "cylinder outside x") && ok;
    ok      = expectEqual(area.isPointInside(10.0f, 0.0f, -1.99f), false, "cylinder outside z") && ok;
    return ok;
}

auto testSphericalTriggerArea() -> bool
{
    CSphericalTriggerArea area(303, 1.0f, 2.0f, 3.0f, 5.0f);

    bool ok = true;
    ok      = expectEqual(area.isPointInside(1.0f, 2.0f, 3.0f), true, "sphere center") && ok;
    ok      = expectEqual(area.isPointInside(4.0f, 6.0f, 3.0f), true, "sphere 3-4-5 boundary") && ok;
    ok      = expectEqual(area.isPointInside(position_t{ 1.0f, 2.0f, 8.0f, 0, 0 }), true, "sphere z boundary position") && ok;
    ok      = expectEqual(area.isPointInside(1.0f, 2.0f, 8.01f), false, "sphere outside z") && ok;
    ok      = expectEqual(area.isPointInside(4.01f, 6.0f, 3.0f), false, "sphere outside diagonal") && ok;
    return ok;
}

} // namespace

auto runTriggerAreaSelfTests() -> bool
{
    bool ok = true;
    ok      = testTriggerAreaIdentityAndCount() && ok;
    ok      = testCuboidTriggerArea() && ok;
    ok      = testCylindricalTriggerArea() && ok;
    ok      = testSphericalTriggerArea() && ok;
    return ok;
}
