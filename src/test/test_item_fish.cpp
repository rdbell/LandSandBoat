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

#include "test_item_fish.h"

#include "map/items/item_fish.h"
#include "map/items/item_general.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item fish self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item fish self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorCopiesBaseItem() -> bool
{
    CItemGeneral base(0x1234);
    base.setStackSize(99);
    base.setQuantity(42);
    base.setBasePrice(777);

    CItemFish fish(base);

    bool ok = true;
    ok      = expectUInt(fish.getID(), 0x1234, "id") && ok;
    ok      = expectBool(fish.isType(ITEM_GENERAL), true, "preserved general type flag") && ok;
    ok      = expectBool(fish.isType(ITEM_PUPPET), false, "puppet type flag") && ok;
    ok      = expectUInt(fish.getStackSize(), 99, "stack size") && ok;
    ok      = expectUInt(fish.getQuantity(), 42, "quantity") && ok;
    ok      = expectUInt(fish.getBasePrice(), 777, "base price") && ok;
    ok      = expectUInt(fish.GetLength(), 0, "default fish length") && ok;
    ok      = expectUInt(fish.GetWeight(), 0, "default fish weight") && ok;
    ok      = expectBool(fish.IsRanked(), false, "default fish rank flag") && ok;
    return ok;
}

auto testFishFields() -> bool
{
    CItemGeneral base(0x2000);
    CItemFish    fish(base);

    bool ok = true;
    fish.SetLength(123);
    fish.SetWeight(456);
    fish.SetRank(true);
    ok = expectUInt(fish.GetLength(), 123, "fish length") && ok;
    ok = expectUInt(fish.GetWeight(), 456, "fish weight") && ok;
    ok = expectBool(fish.IsRanked(), true, "fish ranked") && ok;

    fish.SetRank(false);
    ok = expectBool(fish.IsRanked(), false, "fish rank cleared") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemGeneral base(0x2001);
    CItemFish    fish(base);

    bool ok = true;
    fish.setQuantity(1);
    ok = expectUInt(fish.getQuantity(), 0, "quantity before stack size") && ok;

    fish.setStackSize(12);
    fish.setQuantity(12);
    ok = expectUInt(fish.getQuantity(), 12, "quantity at stack boundary") && ok;

    fish.setQuantity(13);
    ok = expectUInt(fish.getQuantity(), 12, "quantity clamped to stack") && ok;

    fish.setStackSize(1);
    ok = expectUInt(fish.getQuantity(), 12, "quantity after shrinking stack size") && ok;
    return ok;
}

auto testConstructorCopiesFishExdata() -> bool
{
    CItemGeneral base(0x2222);
    CItemFish    original(base);
    original.setStackSize(88);
    original.setQuantity(44);
    original.SetLength(321);
    original.SetWeight(654);
    original.SetRank(true);

    const CItem& copiedBase = original;
    CItemFish    copy(copiedBase);
    original.setQuantity(1);
    original.SetLength(1);
    original.SetWeight(1);
    original.SetRank(false);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_GENERAL), true, "copy preserved general type flag") && ok;
    ok      = expectUInt(copy.getStackSize(), 88, "copy stack size") && ok;
    ok      = expectUInt(copy.getQuantity(), 44, "copy quantity") && ok;
    ok      = expectUInt(copy.GetLength(), 321, "copy fish length") && ok;
    ok      = expectUInt(copy.GetWeight(), 654, "copy fish weight") && ok;
    ok      = expectBool(copy.IsRanked(), true, "copy fish ranked") && ok;
    return ok;
}

} // namespace

auto runItemFishSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorCopiesBaseItem() && ok;
    ok      = testFishFields() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testConstructorCopiesFishExdata() && ok;
    return ok;
}
