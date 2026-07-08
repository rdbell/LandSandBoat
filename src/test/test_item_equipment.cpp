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

#include "test_item_equipment.h"

#include "map/items/item_equipment.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item equipment self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectInt(std::int64_t actual, std::int64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item equipment self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item equipment self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testConstructorDefaults() -> bool
{
    CItemEquipment item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_USABLE), true, "usable type flag") && ok;
    ok      = expectBool(item.isType(ITEM_EQUIPMENT), true, "equipment type flag") && ok;
    ok      = expectBool(item.isType(ITEM_GENERAL), false, "general type flag") && ok;
    ok      = expectUInt(item.getReqLvl(), 255, "default required level") && ok;
    ok      = expectUInt(item.getILvl(), 0, "default item level") && ok;
    ok      = expectUInt(item.getJobs(), 0, "default jobs") && ok;
    ok      = expectUInt(item.getModelId(), 0, "default model id") && ok;
    ok      = expectUInt(item.getScriptType(), SCRIPT_NONE, "default script type") && ok;
    ok      = expectUInt(item.getShieldSize(), 0, "default shield size") && ok;
    ok      = expectBool(item.IsShield(), false, "default not shield") && ok;
    ok      = expectUInt(item.getEquipSlotId(), 255, "default equip slot") && ok;
    ok      = expectUInt(item.getSlotType(), 7, "default slot type") && ok;
    ok      = expectUInt(item.getRemoveSlotId(), 0, "default remove slot") && ok;
    ok      = expectUInt(item.getRemoveSlotLookId(), 0, "default remove look slot") && ok;
    ok      = expectUInt(item.getShieldAbsorption(), 0, "default shield absorption") && ok;
    ok      = expectUInt(item.getSuperiorLevel(), 0, "default superior level") && ok;
    ok      = expectBool(item.modList.empty(), true, "default modifier list") && ok;
    ok      = expectBool(item.petModList.empty(), true, "default pet modifier list") && ok;
    ok      = expectBool(item.latentList.empty(), true, "default latent list") && ok;
    return ok;
}

auto testScalarFieldsAndSlotType() -> bool
{
    CItemEquipment item(0x2000);

    item.setReqLvl(75);
    item.setILvl(119);
    item.setJobs(0x00123456);
    item.setModelId(0x3456);
    item.setScriptType(SCRIPT_EQUIP | SCRIPT_CHANGEZONE | SCRIPT_TIME_DUSK);
    item.setEquipSlotId(0);
    item.setRemoveSlotId(0x0003);
    item.setRemoveSlotLookId(0x003C);
    item.setSuperiorLevel(5);

    bool ok = true;
    ok      = expectUInt(item.getReqLvl(), 75, "required level") && ok;
    ok      = expectUInt(item.getILvl(), 119, "item level") && ok;
    ok      = expectUInt(item.getJobs(), 0x00123456, "jobs") && ok;
    ok      = expectUInt(item.getModelId(), 0x3456, "model id") && ok;
    ok      = expectUInt(item.getScriptType(), SCRIPT_EQUIP | SCRIPT_CHANGEZONE | SCRIPT_TIME_DUSK, "script type") && ok;
    ok      = expectUInt(item.getEquipSlotId(), 0, "zero equip slot") && ok;
    ok      = expectUInt(item.getSlotType(), 0, "zero slot type") && ok;
    ok      = expectUInt(item.getRemoveSlotId(), 0x0003, "remove slot") && ok;
    ok      = expectUInt(item.getRemoveSlotLookId(), 0x003C, "remove look slot") && ok;
    ok      = expectUInt(item.getSuperiorLevel(), 5, "superior level") && ok;

    item.setEquipSlotId(1);
    ok = expectUInt(item.getSlotType(), 0, "slot type bit 0") && ok;
    item.setEquipSlotId(2);
    ok = expectUInt(item.getSlotType(), 1, "slot type bit 1") && ok;
    item.setEquipSlotId(0x0400);
    ok = expectUInt(item.getSlotType(), 10, "slot type bit 10") && ok;
    item.setEquipSlotId(0xFFFF);
    ok = expectUInt(item.getSlotType(), 15, "slot type all bits") && ok;
    return ok;
}

auto testModifiersAndShieldAbsorption() -> bool
{
    CItemEquipment item(0x2001);

    bool ok = true;
    item.addModifier(Mod::DEF, 10);
    item.addModifier(Mod::DEF, 7);
    item.addModifier(Mod::HP, -3);
    ok = expectInt(item.getModifier(Mod::DEF), 17, "summed defense modifier") && ok;
    ok = expectInt(item.getModifier(Mod::HP), -3, "negative hp modifier") && ok;
    ok = expectUInt(item.getShieldAbsorption(), 0, "non-shield absorption unchanged") && ok;

    ok = expectBool(item.delModifier(Mod::DEF, 99), false, "delete missing modifier") && ok;
    ok = expectBool(item.delModifier(Mod::DEF, 10), true, "delete first matching modifier") && ok;
    ok = expectInt(item.getModifier(Mod::DEF), 7, "modifier after delete") && ok;

    item.setShieldSize(1);
    ok = expectBool(item.IsShield(), true, "shield size one") && ok;
    item.addModifier(Mod::DEF, 20);
    ok = expectUInt(item.getShieldAbsorption(), 32, "buckler absorption") && ok;

    item.setShieldSize(3);
    item.addModifier(Mod::DEF, 20);
    ok = expectUInt(item.getShieldAbsorption(), 60, "kite absorption") && ok;

    item.setShieldSize(5);
    item.addModifier(Mod::DEF, 200);
    ok = expectUInt(item.getShieldAbsorption(), 100, "absorption cap") && ok;

    item.setShieldSize(7);
    ok = expectBool(item.IsShield(), false, "shield size seven") && ok;
    item.addModifier(Mod::DEF, 20);
    ok = expectUInt(item.getShieldAbsorption(), 100, "non-shield size leaves absorption") && ok;
    return ok;
}

auto testRacePetAndLatentLists() -> bool
{
    CItemEquipment item(0x2002);

    bool ok = true;
    ok      = expectBool(item.isEquippableByRace(1), true, "default race one") && ok;
    ok      = expectBool(item.isEquippableByRace(8), true, "default race eight") && ok;

    item.addModifier(Mod::EQUIPMENT_ONLY_RACE, (1 << 1) | (1 << 3));
    ok = expectBool(item.isEquippableByRace(1), false, "restricted race one") && ok;
    ok = expectBool(item.isEquippableByRace(2), true, "restricted race two") && ok;
    ok = expectBool(item.isEquippableByRace(4), true, "restricted race four") && ok;
    ok = expectBool(item.isEquippableByRace(8), false, "restricted race eight") && ok;

    item.addPetModifier(Mod::HP, PetModType::Avatar, 12);
    item.addPetModifier(Mod::HP, PetModType::Wyvern, 12);
    item.addPetModifier(Mod::DEF, PetModType::Avatar, 9);
    ok = expectUInt(item.petModList.size(), 3, "pet modifier count") && ok;
    ok = expectBool(item.delPetModifier(Mod::HP, PetModType::Automaton, 12), false, "delete missing pet modifier") && ok;
    ok = expectBool(item.delPetModifier(Mod::HP, PetModType::Avatar, 12), true, "delete pet modifier") && ok;
    ok = expectUInt(item.petModList.size(), 2, "pet modifier count after delete") && ok;
    ok = expectBool(item.petModList[0].getPetModType() == PetModType::Wyvern, true, "first remaining pet type") && ok;

    item.addLatent(xi::Latent::HpUnderPercent, 75, Mod::DEF, 14);
    ok = expectUInt(item.latentList.size(), 1, "latent count") && ok;
    ok = expectBool(item.latentList[0].ConditionsID == xi::Latent::HpUnderPercent, true, "latent condition") && ok;
    ok = expectUInt(item.latentList[0].ConditionsValue, 75, "latent condition value") && ok;
    ok = expectBool(item.latentList[0].ModValue == Mod::DEF, true, "latent mod") && ok;
    ok = expectInt(item.latentList[0].ModPower, 14, "latent power") && ok;
    return ok;
}

auto testCopyConstructorCopiesFields() -> bool
{
    CItemEquipment original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setReqLvl(75);
    original.setILvl(119);
    original.setJobs(0x00123456);
    original.setModelId(0x3456);
    original.setShieldSize(2);
    original.setScriptType(SCRIPT_EQUIP | SCRIPT_TIME_DAY);
    original.setEquipSlotId(0x0400);
    original.setRemoveSlotId(0x0003);
    original.setRemoveSlotLookId(0x003C);
    original.setSuperiorLevel(5);
    original.addModifier(Mod::DEF, 20);
    original.addModifier(Mod::HP, -3);
    original.addPetModifier(Mod::HP, PetModType::Avatar, 12);
    original.addLatent(xi::Latent::HpUnderPercent, 75, Mod::DEF, 14);

    CItemEquipment copy(original);
    original.setQuantity(1);
    original.setReqLvl(1);
    original.setILvl(1);
    original.setJobs(1);
    original.setModelId(1);
    original.setShieldSize(0);
    original.setScriptType(SCRIPT_NONE);
    original.setEquipSlotId(1);
    original.setRemoveSlotId(1);
    original.setRemoveSlotLookId(1);
    original.setSuperiorLevel(1);
    original.delModifier(Mod::DEF, 20);
    original.delPetModifier(Mod::HP, PetModType::Avatar, 12);
    original.latentList.clear();

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_USABLE), true, "copy usable type flag") && ok;
    ok      = expectBool(copy.isType(ITEM_EQUIPMENT), true, "copy equipment type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getReqLvl(), 75, "copy required level") && ok;
    ok      = expectUInt(copy.getILvl(), 119, "copy item level") && ok;
    ok      = expectUInt(copy.getJobs(), 0x00123456, "copy jobs") && ok;
    ok      = expectUInt(copy.getModelId(), 0x3456, "copy model id") && ok;
    ok      = expectUInt(copy.getShieldSize(), 2, "copy shield size") && ok;
    ok      = expectUInt(copy.getShieldAbsorption(), 50, "copy shield absorption") && ok;
    ok      = expectUInt(copy.getScriptType(), SCRIPT_EQUIP | SCRIPT_TIME_DAY, "copy script type") && ok;
    ok      = expectUInt(copy.getEquipSlotId(), 0x0400, "copy equip slot") && ok;
    ok      = expectUInt(copy.getSlotType(), 10, "copy slot type") && ok;
    ok      = expectUInt(copy.getRemoveSlotId(), 0x0003, "copy remove slot") && ok;
    ok      = expectUInt(copy.getRemoveSlotLookId(), 0x003C, "copy remove look slot") && ok;
    ok      = expectUInt(copy.getSuperiorLevel(), 5, "copy superior level") && ok;
    ok      = expectInt(copy.getModifier(Mod::DEF), 20, "copy defense modifier") && ok;
    ok      = expectInt(copy.getModifier(Mod::HP), -3, "copy hp modifier") && ok;
    ok      = expectUInt(copy.petModList.size(), 1, "copy pet modifier count") && ok;
    ok      = expectUInt(copy.latentList.size(), 1, "copy latent count") && ok;
    return ok;
}

} // namespace

auto runItemEquipmentSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testScalarFieldsAndSlotType() && ok;
    ok      = testModifiersAndShieldAbsorption() && ok;
    ok      = testRacePetAndLatentLists() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
