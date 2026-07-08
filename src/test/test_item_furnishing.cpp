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

#include "test_item_furnishing.h"

#include "map/items/item_furnishing.h"

#include <cstdint>
#include <iostream>
#include <string>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item furnishing self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectString(const std::string& actual, const std::string& expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item furnishing self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item furnishing self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorDefaults() -> bool
{
    CItemFurnishing item(0x1234);
    const auto      size = item.size();

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_FURNISHING), true, "furnishing type flag") && ok;
    ok      = expectBool(item.isType(ITEM_GENERAL), false, "general type flag") && ok;
    ok      = expectUInt(item.getStackSize(), 0, "default stack size") && ok;
    ok      = expectUInt(item.getQuantity(), 0, "default quantity") && ok;
    ok      = expectUInt(item.getStorage(), 0, "default storage") && ok;
    ok      = expectUInt(item.getMoghancement(), 0, "default moghancement") && ok;
    ok      = expectUInt(item.getElement(), 0, "default element") && ok;
    ok      = expectUInt(item.getAura(), 0, "default aura") && ok;
    ok      = expectUInt(size.first, 1, "default size x") && ok;
    ok      = expectUInt(size.second, 1, "default size y") && ok;
    ok      = expectUInt(item.height(), 0, "default height") && ok;
    ok      = expectUInt(static_cast<uint8>(item.placement()), static_cast<uint8>(FurnishingPlacement::Floor), "default placement") && ok;
    ok      = expectBool(item.isInstalled(), false, "default installed") && ok;
    ok      = expectUInt(item.getCol(), 0, "default col") && ok;
    ok      = expectUInt(item.getRow(), 0, "default row") && ok;
    ok      = expectUInt(item.getLevel(), 0, "default level") && ok;
    ok      = expectUInt(item.getRotation(), 0, "default rotation") && ok;
    ok      = expectUInt(item.getOrder(), 0, "default order") && ok;
    ok      = expectBool(item.getOn2ndFloor(), false, "default second floor") && ok;
    ok      = expectUInt(item.getMannequinRace(), 0, "default mannequin race") && ok;
    ok      = expectUInt(item.getMannequinPose(), 0, "default mannequin pose") && ok;
    ok      = expectString(item.getSignature(), "", "default signature") && ok;
    ok      = expectBool(item.isGardeningPot(), false, "default gardening pot") && ok;
    return ok;
}

auto testScalarFields() -> bool
{
    CItemFurnishing item(0x2000);

    bool ok = true;
    item.setStorage(79);
    ok = expectUInt(item.getStorage(), 79, "storage below cap") && ok;
    item.setStorage(81);
    ok = expectUInt(item.getStorage(), 80, "storage cap") && ok;

    item.setMoghancement(MOGHANCEMENT_FISHING);
    item.setElement(6);
    item.setAura(7);
    item.setSize(3, 4);
    item.setHeight(55);
    item.setPlacement(FurnishingPlacement::Wall);

    const auto size = item.size();
    ok              = expectUInt(item.getMoghancement(), MOGHANCEMENT_FISHING, "moghancement") && ok;
    ok              = expectUInt(item.getElement(), 6, "element") && ok;
    ok              = expectUInt(item.getAura(), 7, "aura") && ok;
    ok              = expectUInt(size.first, 3, "size x") && ok;
    ok              = expectUInt(size.second, 4, "size y") && ok;
    ok              = expectUInt(item.height(), 55, "height") && ok;
    ok              = expectUInt(static_cast<uint8>(item.placement()), static_cast<uint8>(FurnishingPlacement::Wall), "placement") && ok;
    return ok;
}

auto testFurnitureExdataFields() -> bool
{
    CItemFurnishing item(0x2001);

    bool ok = true;
    item.setInstalled(true);
    item.setOn2ndFloor(true);
    item.setCol(1);
    item.setRow(2);
    item.setLevel(3);
    item.setRotation(4);
    item.setOrder(5);

    ok = expectBool(item.isInstalled(), true, "installed") && ok;
    ok = expectBool(item.getOn2ndFloor(), true, "second floor") && ok;
    ok = expectUInt(item.getCol(), 1, "col") && ok;
    ok = expectUInt(item.getRow(), 2, "row") && ok;
    ok = expectUInt(item.getLevel(), 3, "level") && ok;
    ok = expectUInt(item.getRotation(), 4, "rotation") && ok;
    ok = expectUInt(item.getOrder(), 5, "order") && ok;

    item.setInstalled(false);
    item.setOn2ndFloor(false);
    ok = expectBool(item.isInstalled(), false, "installed cleared") && ok;
    ok = expectBool(item.getOn2ndFloor(), false, "second floor cleared") && ok;
    return ok;
}

auto testSignatureAndMannequinExdata() -> bool
{
    CItemFurnishing item(0x2002);

    bool ok = true;
    item.setSignature("OmegaXI2026");
    ok = expectString(item.getSignature(), "OmegaXI2026", "signature") && ok;

    item.m_extra[20] = 0xAA;
    item.m_extra[21] = 0xBB;
    item.setMannequinRace(12);
    item.setMannequinPose(34);
    ok = expectUInt(item.getMannequinRace(), 12, "mannequin race") && ok;
    ok = expectUInt(item.getMannequinPose(), 34, "mannequin pose") && ok;
    ok = expectUInt(item.m_extra[18], 12, "mannequin race raw offset") && ok;
    ok = expectUInt(item.m_extra[19], 34, "mannequin pose raw offset") && ok;
    ok = expectUInt(item.m_extra[20], 0xAA, "post-mannequin signature byte 10") && ok;
    ok = expectUInt(item.m_extra[21], 0xBB, "post-mannequin signature byte 11") && ok;

    item.setSignature("AB_cd!");
    ok = expectString(item.getSignature(), "AB", "invalid signature truncates") && ok;
    return ok;
}

auto testQuantityClampUsesCurrentStackSize() -> bool
{
    CItemFurnishing item(0x2003);

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
    CItemFurnishing original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setBasePrice(777);
    original.setStorage(80);
    original.setMoghancement(MOGHANCEMENT_GARDENING);
    original.setElement(4);
    original.setAura(5);
    original.setSize(6, 7);
    original.setHeight(88);
    original.setPlacement(FurnishingPlacement::Surface);
    original.setInstalled(true);
    original.setOn2ndFloor(true);
    original.setCol(9);
    original.setRow(10);
    original.setLevel(11);
    original.setRotation(12);
    original.setOrder(13);
    original.setMannequinRace(14);
    original.setMannequinPose(15);

    CItemFurnishing copy(original);
    original.setQuantity(1);
    original.setBasePrice(1);
    original.setStackSize(1);
    original.setStorage(1);
    original.setMoghancement(1);
    original.setElement(1);
    original.setAura(1);
    original.setSize(1, 1);
    original.setHeight(1);
    original.setPlacement(FurnishingPlacement::OnTable);
    original.setInstalled(false);
    original.setOn2ndFloor(false);
    original.setCol(1);
    original.setRow(1);
    original.setLevel(1);
    original.setRotation(1);
    original.setOrder(1);
    original.setMannequinRace(1);
    original.setMannequinPose(1);

    const auto size = copy.size();
    bool       ok   = true;
    ok             = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok             = expectBool(copy.isType(ITEM_FURNISHING), true, "copy furnishing type flag") && ok;
    ok             = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok             = expectUInt(copy.getBasePrice(), 777, "copy base price") && ok;
    ok             = expectUInt(copy.getStackSize(), 99, "copy stack size") && ok;
    ok             = expectUInt(copy.getStorage(), 80, "copy storage") && ok;
    ok             = expectUInt(copy.getMoghancement(), MOGHANCEMENT_GARDENING, "copy moghancement") && ok;
    ok             = expectUInt(copy.getElement(), 4, "copy element") && ok;
    ok             = expectUInt(copy.getAura(), 5, "copy aura") && ok;
    ok             = expectUInt(size.first, 6, "copy size x") && ok;
    ok             = expectUInt(size.second, 7, "copy size y") && ok;
    ok             = expectUInt(copy.height(), 88, "copy height") && ok;
    ok             = expectUInt(static_cast<uint8>(copy.placement()), static_cast<uint8>(FurnishingPlacement::Surface), "copy placement") && ok;
    ok             = expectBool(copy.isInstalled(), true, "copy installed") && ok;
    ok             = expectBool(copy.getOn2ndFloor(), true, "copy second floor") && ok;
    ok             = expectUInt(copy.getCol(), 9, "copy col") && ok;
    ok             = expectUInt(copy.getRow(), 10, "copy row") && ok;
    ok             = expectUInt(copy.getLevel(), 11, "copy level") && ok;
    ok             = expectUInt(copy.getRotation(), 12, "copy rotation") && ok;
    ok             = expectUInt(copy.getOrder(), 13, "copy order") && ok;
    ok             = expectUInt(copy.getMannequinRace(), 14, "copy mannequin race") && ok;
    ok             = expectUInt(copy.getMannequinPose(), 15, "copy mannequin pose") && ok;
    return ok;
}

} // namespace

auto runItemFurnishingSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testScalarFields() && ok;
    ok      = testFurnitureExdataFields() && ok;
    ok      = testSignatureAndMannequinExdata() && ok;
    ok      = testQuantityClampUsesCurrentStackSize() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
