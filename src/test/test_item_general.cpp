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

#include "test_item_general.h"

#include "map/items/item_general.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item general self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item general self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item general self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorDefaults() -> bool
{
    CItemGeneral item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_GENERAL), true, "general type flag") && ok;
    ok      = expectBool(item.isType(ITEM_CURRENCY), false, "currency type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 0, "default stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getSubID(), 0, "default sub id") && ok;
    ok      = expectUInt(static_cast<uint32>(item.getFlag()), static_cast<uint32>(ItemFlag::None), "default flag") && ok;
    ok      = expectUInt(item.getAppraisalID(), 0, "default appraisal id") && ok;
    ok      = expectUInt(item.getAHCat(), 0, "default auction category") && ok;
    ok      = expectUInt(item.getReserve(), 0, "default reserve") && ok;
    ok      = expectUInt(item.getBasePrice(), 0, "default base price") && ok;
    ok      = expectUInt(item.getCharPrice(), 0, "default char price") && ok;
    ok      = expectUInt(item.getLocationID(), 0xFF, "default location id") && ok;
    ok      = expectUInt(item.getSlotID(), 0xFF, "default slot id") && ok;
    ok      = expectBool(item.isSent(), false, "default sent flag") && ok;
    ok      = expectBool(item.isDirty(), false, "default dirty flag") && ok;
    ok      = expectBool(item.isStorageSlip(), false, "default storage slip") && ok;
    ok      = expectBool(item.isSoultrapper(), false, "default soultrapper") && ok;
    ok      = expectBool(item.isMannequin(), false, "default mannequin") && ok;
    ok      = expectString(item.getName(), "", "default name") && ok;
    ok      = expectString(item.getSender(), "", "default sender") && ok;
    ok      = expectString(item.getReceiver(), "", "default receiver") && ok;
    ok      = expectString(item.getSignature(), "", "default signature") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemGeneral item(0x2000);

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

auto testBaseStateAndExdata() -> bool
{
    CItemGeneral item(0x2001);

    bool ok = true;
    item.setID(0x2222);
    ok = expectUInt(item.getID(), 0x2222, "id setter") && ok;
    item.setSubID(0x3333);
    ok = expectUInt(item.getSubID(), 0x3333, "sub id") && ok;

    item.setSubType(ITEM_LOCKED | ITEM_CHARGED | ITEM_AUGMENTED);
    ok = expectBool(item.isSubType(ITEM_LOCKED), true, "locked subtype") && ok;
    ok = expectBool(item.isSubType(ITEM_CHARGED), true, "charged subtype") && ok;
    ok = expectBool(item.isSubType(ITEM_AUGMENTED), true, "augmented subtype") && ok;
    ok = expectBool(item.isSubType(ITEM_NORMAL), false, "normal subtype") && ok;
    item.setSubType(ITEM_UNLOCKED);
    ok = expectBool(item.isSubType(ITEM_LOCKED), false, "unlocked clears locked") && ok;
    ok = expectBool(item.isSubType(ITEM_CHARGED), true, "unlocked keeps charged") && ok;
    ok = expectBool(item.isSubType(ITEM_AUGMENTED), true, "unlocked keeps augmented") && ok;

    item.setFlag(ItemFlag::CanUse | ItemFlag::Rare);
    ok = expectUInt(static_cast<uint32>(item.getFlag()), static_cast<uint32>(ItemFlag::CanUse | ItemFlag::Rare), "flag mask") && ok;
    ok = expectBool(item.hasFlag(ItemFlag::CanUse), true, "has can use flag") && ok;
    ok = expectBool(item.hasFlag(ItemFlag::CanUse | ItemFlag::Exclusive), true, "has any overlapping flag") && ok;
    ok = expectBool(item.hasFlag(ItemFlag::Exclusive), false, "missing exclusive flag") && ok;

    item.setStackSize(12);
    item.setQuantity(12);
    item.setReserve(99);
    ok = expectUInt(item.getReserve(), 12, "reserve clamped to quantity") && ok;
    item.setReserve(3);
    ok = expectUInt(item.getReserve(), 3, "reserve below quantity") && ok;

    item.setAHCat(7);
    item.setBasePrice(123);
    item.setCharPrice(500);
    item.setFlag(ItemFlag::Exclusive);
    item.setCharPrice(600);
    ok = expectUInt(item.getAHCat(), 7, "auction category") && ok;
    ok = expectUInt(item.getBasePrice(), 123, "base price") && ok;
    ok = expectUInt(item.getCharPrice(), 500, "exclusive preserves char price") && ok;
    item.setFlag(ItemFlag::None);
    item.setCharPrice(600);
    ok = expectUInt(item.getCharPrice(), 600, "non-exclusive char price") && ok;

    item.setName("Potion");
    item.setSender("Sender");
    item.setReceiver("Receiver");
    item.setLocationID(8);
    item.setSlotID(9);
    item.setSent(true);
    item.setDirty(true);
    ok = expectString(item.getName(), "Potion", "name") && ok;
    ok = expectString(item.getSender(), "Sender", "sender") && ok;
    ok = expectString(item.getReceiver(), "Receiver", "receiver") && ok;
    ok = expectUInt(item.getLocationID(), 8, "location id") && ok;
    ok = expectUInt(item.getSlotID(), 9, "slot id") && ok;
    ok = expectBool(item.isSent(), true, "sent flag") && ok;
    ok = expectBool(item.isDirty(), true, "dirty flag") && ok;

    item.m_extra[11] = 0xAB;
    item.m_extra[21] = 0xCD;
    item.m_extra[23] = 0xEF;
    item.setAppraisalID(0x44);
    ok = expectUInt(item.getAppraisalID(), 0x44, "appraisal id") && ok;
    ok = expectUInt(item.m_extra[21], 0xCD, "appraisal preserves previous byte") && ok;
    ok = expectUInt(item.m_extra[22], 0x44, "appraisal raw offset") && ok;
    ok = expectUInt(item.m_extra[23], 0xEF, "appraisal preserves next byte") && ok;

    item.setSignature("Base2026");
    ok = expectString(item.getSignature(), "Base2026", "signature") && ok;
    ok = expectUInt(item.m_extra[11], 0xAB, "signature preserves prefix") && ok;
    item.setSignature("AB_cd!");
    ok = expectString(item.getSignature(), "AB", "invalid signature truncates") && ok;
    return ok;
}

auto testBaseClassifiers() -> bool
{
    bool ok = true;

    ok = expectBool(CItemGeneral(29312).isStorageSlip(), true, "storage slip low") && ok;
    ok = expectBool(CItemGeneral(29339).isStorageSlip(), true, "storage slip high") && ok;
    ok = expectBool(CItemGeneral(29311).isStorageSlip(), false, "storage slip before") && ok;
    ok = expectBool(CItemGeneral(29340).isStorageSlip(), false, "storage slip after") && ok;
    ok = expectBool(CItemGeneral(18721).isSoultrapper(), true, "soultrapper") && ok;
    ok = expectBool(CItemGeneral(18724).isSoultrapper(), true, "soultrapper 2000") && ok;
    ok = expectBool(CItemGeneral(18722).isSoultrapper(), false, "not soultrapper") && ok;
    ok = expectBool(CItemGeneral(256).isMannequin(), true, "mannequin low") && ok;
    ok = expectBool(CItemGeneral(263).isMannequin(), true, "mannequin high") && ok;
    ok = expectBool(CItemGeneral(264).isMannequin(), false, "not mannequin") && ok;
    return ok;
}

auto testCopyConstructorCopiesBaseFields() -> bool
{
    CItemGeneral original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setBasePrice(777);
    original.setSubID(0x3333);
    original.setSubType(ITEM_CHARGED);
    original.setReserve(12);
    original.setCharPrice(55);
    original.setAHCat(9);
    original.setFlag(ItemFlag::CanUse | ItemFlag::Rare);
    original.setLocationID(10);
    original.setSlotID(11);
    original.setSent(true);
    original.setDirty(true);
    original.setName("Original");
    original.setSender("Sender");
    original.setReceiver("Receiver");
    original.setSignature("CopySig");

    CItemGeneral copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);
    original.setSubID(1);
    original.setSubType(ITEM_UNLOCKED);
    original.setReserve(1);
    original.setCharPrice(1);
    original.setAHCat(1);
    original.setFlag(ItemFlag::None);
    original.setLocationID(1);
    original.setSlotID(1);
    original.setSent(false);
    original.setDirty(false);
    original.setName("Changed");
    original.setSender("Changed");
    original.setReceiver("Changed");
    original.setSignature("Changed");

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_GENERAL), true, "copy general type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok      = expectUInt(copy.getStackSize(), 99, "copy stack size") && ok;
    ok      = expectUInt(copy.getSubID(), 0x3333, "copy sub id") && ok;
    ok      = expectBool(copy.isSubType(ITEM_CHARGED), true, "copy subtype") && ok;
    ok      = expectUInt(copy.getReserve(), 12, "copy reserve") && ok;
    ok      = expectUInt(copy.getCharPrice(), 55, "copy char price") && ok;
    ok      = expectUInt(copy.getAHCat(), 9, "copy auction category") && ok;
    ok      = expectUInt(static_cast<uint32>(copy.getFlag()), static_cast<uint32>(ItemFlag::CanUse | ItemFlag::Rare), "copy flag") && ok;
    ok      = expectUInt(copy.getLocationID(), 10, "copy location id") && ok;
    ok      = expectUInt(copy.getSlotID(), 11, "copy slot id") && ok;
    ok      = expectBool(copy.isSent(), true, "copy sent flag") && ok;
    ok      = expectBool(copy.isDirty(), true, "copy dirty flag") && ok;
    ok      = expectString(copy.getName(), "Original", "copy name") && ok;
    ok      = expectString(copy.getSender(), "Sender", "copy sender") && ok;
    ok      = expectString(copy.getReceiver(), "Receiver", "copy receiver") && ok;
    ok      = expectString(copy.getSignature(), "CopySig", "copy signature") && ok;
    return ok;
}

} // namespace

auto runItemGeneralSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testBaseStateAndExdata() && ok;
    ok      = testBaseClassifiers() && ok;
    ok      = testCopyConstructorCopiesBaseFields() && ok;
    return ok;
}
