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

#include "test_item_puppet.h"

#include "map/items/item_puppet.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item puppet self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item puppet self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorDefaults() -> bool
{
    CItemPuppet item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_PUPPET), true, "puppet type flag") && ok;
    ok      = expectBool(item.isType(ITEM_USABLE), false, "usable type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 0, "default stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getEquipSlot(), 0, "default equip slot") && ok;
    ok      = expectUInt(item.getElementSlots(), 0, "default element slots") && ok;
    return ok;
}

auto testPuppetFields() -> bool
{
    CItemPuppet item(0x2000);

    bool ok = true;
    item.setEquipSlot(ITEM_PUPPET_HEAD);
    ok = expectUInt(item.getEquipSlot(), ITEM_PUPPET_HEAD, "head equip slot") && ok;

    item.setEquipSlot(0x123);
    ok = expectUInt(item.getEquipSlot(), 0x23, "equip slot truncates to uint8") && ok;

    item.setElementSlots(0x01020304);
    ok = expectUInt(item.getElementSlots(), 0x01020304, "element slots") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemPuppet item(0x2001);

    bool ok = true;
    item.setQuantity(1);
    ok = expectUInt(item.getQuantity(), 0, "quantity before stack size") && ok;

    item.setStackSize(12);
    item.setQuantity(12);
    ok = expectUInt(item.getQuantity(), 12, "quantity at stack boundary") && ok;

    item.setQuantity(13);
    ok = expectUInt(item.getQuantity(), 12, "quantity clamped to stack") && ok;

    item.setStackSize(1);
    ok = expectUInt(item.getQuantity(), 12, "quantity after shrinking stack size") && ok;
    return ok;
}

auto testCopyConstructorCopiesFields() -> bool
{
    CItemPuppet original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setBasePrice(777);
    original.setEquipSlot(ITEM_PUPPET_ATTACHMENT);
    original.setElementSlots(0x0A0B0C0D);

    CItemPuppet copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);
    original.setEquipSlot(ITEM_PUPPET_FRAME);
    original.setElementSlots(1);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_PUPPET), true, "copy puppet type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok      = expectUInt(copy.getStackSize(), 99, "copy stack size") && ok;
    ok      = expectUInt(copy.getEquipSlot(), ITEM_PUPPET_ATTACHMENT, "copy equip slot") && ok;
    ok      = expectUInt(copy.getElementSlots(), 0x0A0B0C0D, "copy element slots") && ok;
    return ok;
}

} // namespace

auto runItemPuppetSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testPuppetFields() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
