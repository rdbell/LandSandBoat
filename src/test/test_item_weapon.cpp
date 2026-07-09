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

#include "test_item_weapon.h"

#include "map/items/item_weapon.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item weapon self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectInt(std::int64_t actual, std::int64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item weapon self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "item weapon self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectDouble(double actual, double expected, const char* label) -> bool
{
    if (std::abs(actual - expected) > 0.000001)
    {
        std::cerr << "item weapon self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto exdataU16(const CItemWeapon& item, const std::size_t offset) -> std::uint16_t
{
    std::uint16_t value = 0;
    std::memcpy(&value, item.m_extra + offset, sizeof(value));
    return value;
}

auto testConstructorDefaults() -> bool
{
    CItemWeapon item(0x1234);

    bool ok = true;
    ok      = expectUInt(item.getID(), 0x1234, "id") && ok;
    ok      = expectBool(item.isType(ITEM_USABLE), true, "usable type flag") && ok;
    ok      = expectBool(item.isType(ITEM_EQUIPMENT), true, "equipment type flag") && ok;
    ok      = expectBool(item.isType(ITEM_WEAPON), true, "weapon type flag") && ok;
    ok      = expectUInt(item.getSkillType(), SKILL_NONE, "default skill") && ok;
    ok      = expectUInt(item.getSubSkillType(), SUBSKILL_XBOW_SHORTBOW, "default subskill") && ok;
    ok      = expectUInt(item.getILvlSkill(), 0, "default ilvl skill") && ok;
    ok      = expectUInt(item.getILvlParry(), 0, "default ilvl parry") && ok;
    ok      = expectUInt(item.getILvlMacc(), 0, "default ilvl macc") && ok;
    ok      = expectUInt(item.getDamage(), 0, "default damage") && ok;
    ok      = expectUInt(item.getDelay(), 8000, "default delay milliseconds") && ok;
    ok      = expectUInt(item.getBaseDelay(), 480, "default base delay game units") && ok;
    ok      = expectUInt(static_cast<uint16>(item.getDmgType()), static_cast<uint16>(xi::DamageType::None), "default damage type") && ok;
    ok      = expectUInt(item.getAdditionalEffect(), 0, "default additional effect") && ok;
    ok      = expectUInt(item.getHitCount(), 1, "default hit count") && ok;
    ok      = expectDouble(item.getDPS(), 0.0, "default dps") && ok;
    ok      = expectBool(item.isRanged(), false, "default ranged") && ok;
    ok      = expectBool(item.isThrowing(), false, "default throwing") && ok;
    ok      = expectBool(item.isShuriken(), false, "default shuriken") && ok;
    ok      = expectBool(item.isTwoHanded(), false, "default two-handed") && ok;
    ok      = expectBool(item.isHandToHand(), false, "default hand-to-hand") && ok;
    ok      = expectUInt(item.getTotalUnlockPointsNeeded(), 0, "default unlock total") && ok;
    ok      = expectUInt(item.getCurrentUnlockPoints(), 0, "default unlock current") && ok;
    ok      = expectBool(item.isUnlockable(), false, "default unlockable") && ok;
    ok      = expectBool(item.isUnlocked(), false, "default unlocked") && ok;
    ok      = expectBool(item.isDirty(), false, "default dirty flag") && ok;
    return ok;
}

auto testScalarFieldsAndDelay() -> bool
{
    CItemWeapon item(0x2000);

    item.setSubSkillType(SUBSKILL_GUN);
    item.setILvlSkill(242);
    item.setILvlParry(188);
    item.setILvlMacc(215);
    item.setDelay(240);
    item.setBaseDelay(300);
    item.setDamage(123);
    item.setDmgType(xi::DamageType::Slashing);
    item.setAdditionalEffect(7);
    item.setDPS(12.75);

    bool ok = true;
    ok      = expectUInt(item.getSubSkillType(), SUBSKILL_GUN, "subskill") && ok;
    ok      = expectUInt(item.getILvlSkill(), 242, "ilvl skill") && ok;
    ok      = expectUInt(item.getILvlParry(), 188, "ilvl parry") && ok;
    ok      = expectUInt(item.getILvlMacc(), 215, "ilvl macc") && ok;
    ok      = expectUInt(item.getDelay(), 4000, "delay converted to milliseconds") && ok;
    ok      = expectUInt(item.getBaseDelay(), 300, "base delay converted back to game units") && ok;
    ok      = expectUInt(item.getDamage(), 123, "damage") && ok;
    ok      = expectUInt(static_cast<uint16>(item.getDmgType()), static_cast<uint16>(xi::DamageType::Slashing), "damage type") && ok;
    ok      = expectUInt(item.getAdditionalEffect(), 7, "additional effect") && ok;
    ok      = expectDouble(item.getDPS(), 12.75, "dps") && ok;

    item.resetDelay();
    ok = expectUInt(item.getDelay(), 5000, "reset delay uses converted base delay") && ok;
    return ok;
}

auto testSkillPredicates() -> bool
{
    CItemWeapon item(0x2001);

    bool ok = true;
    item.setSkillType(SKILL_THROWING);
    ok = expectBool(item.isRanged(), true, "throwing sets ranged") && ok;
    ok = expectBool(item.isThrowing(), true, "throwing predicate") && ok;
    ok = expectBool(item.isShuriken(), false, "throwing without shuriken subskill") && ok;
    item.setSubSkillType(SUBSKILL_SHURIKEN);
    ok = expectBool(item.isShuriken(), true, "shuriken predicate") && ok;

    item.setSkillType(SKILL_SWORD);
    ok = expectUInt(item.getSkillType(), SKILL_SWORD, "skill after sword") && ok;
    ok = expectBool(item.isRanged(), true, "ranged flag remains sticky") && ok;
    ok = expectBool(item.isThrowing(), false, "throwing follows current skill") && ok;

    item.setSkillType(SKILL_GREAT_SWORD);
    ok = expectBool(item.isTwoHanded(), true, "great sword sets two-handed") && ok;
    item.setSkillType(SKILL_DAGGER);
    ok = expectBool(item.isTwoHanded(), true, "two-handed flag remains sticky") && ok;

    CItemWeapon h2h(0x2002);
    h2h.setSkillType(SKILL_HAND_TO_HAND);
    ok = expectBool(h2h.isHandToHand(), true, "hand-to-hand predicate") && ok;
    ok = expectBool(h2h.isRanged(), false, "hand-to-hand not ranged") && ok;
    ok = expectBool(h2h.isTwoHanded(), false, "hand-to-hand not two-handed") && ok;
    return ok;
}

auto testUnlockAndRodExdata() -> bool
{
    CItemWeapon item(0x2003);

    bool ok = true;
    item.setTotalUnlockPointsNeeded(300);
    ok = expectBool(item.isUnlockable(), false, "unlock total without skill") && ok;

    item.setSkillType(SKILL_SWORD);
    ok = expectBool(item.isUnlockable(), true, "unlockable with skill and total") && ok;
    ok = expectBool(item.isUnlocked(), false, "not initially unlocked") && ok;
    ok = expectBool(item.isDirty(), false, "clean before unlock point write") && ok;
    item.setCurrentUnlockPoints(120);
    ok = expectUInt(item.getCurrentUnlockPoints(), 120, "current unlock points") && ok;
    ok = expectUInt(exdataU16(item, 0), 120, "raw unlock points") && ok;
    ok = expectBool(item.isDirty(), true, "set current unlock points marks dirty") && ok;
    item.setDirty(false);
    ok = expectBool(item.addWsPoints(50), false, "partial ws points add") && ok;
    ok = expectUInt(item.getCurrentUnlockPoints(), 170, "current unlock after partial add") && ok;
    ok = expectBool(item.isDirty(), true, "partial ws points add marks dirty") && ok;
    item.setDirty(false);
    ok = expectBool(item.addWsPoints(200), true, "capped ws points add") && ok;
    ok = expectUInt(item.getCurrentUnlockPoints(), 300, "current unlock capped") && ok;
    ok = expectBool(item.isUnlocked(), true, "unlocked after cap") && ok;
    ok = expectBool(item.isDirty(), true, "capped ws points add marks dirty") && ok;

    CItemWeapon inert(0x2004);
    ok = expectBool(inert.addWsPoints(10), true, "zero unlock total add returns true") && ok;
    ok = expectUInt(inert.getCurrentUnlockPoints(), 0, "zero unlock total remains zero") && ok;

    CItemWeapon overflow(0x2007);
    overflow.setSkillType(SKILL_SWORD);
    overflow.setTotalUnlockPointsNeeded(65535);
    overflow.setCurrentUnlockPoints(65000);
    ok = expectBool(overflow.addWsPoints(1000), true, "overflowing ws points add returns true") && ok;
    ok = expectUInt(overflow.getCurrentUnlockPoints(), 65535, "overflowing ws points caps") && ok;

    CItemWeapon rod(0x2005);
    rod.setRodNumber(0x3456);
    ok = expectUInt(exdataU16(rod, 0), 0x1002, "rod marker") && ok;
    ok = expectUInt(rod.m_extra[4], 0x63, "rod flag") && ok;
    ok = expectUInt(exdataU16(rod, 6), 0x3456, "rod number") && ok;
    return ok;
}

auto testHitCountBounds() -> bool
{
    CItemWeapon item(0x2006);

    bool ok = true;
    item.setMaxHit(0);
    ok = expectUInt(item.getHitCount(), 1, "zero max hit returns one") && ok;
    item.setMaxHit(1);
    ok = expectUInt(item.getHitCount(), 1, "one max hit returns one") && ok;

    item.setMaxHit(99);
    for (auto i = 0; i < 64; ++i)
    {
        const auto hits = item.getHitCount();
        if (hits < 1 || hits > 8)
        {
            std::cerr << "item weapon self-test failed: clamped random hit count got "
                      << static_cast<int>(hits) << " expected 1..8\n";
            ok = false;
            break;
        }
    }
    return ok;
}

auto testCopyConstructorCopiesFields() -> bool
{
    CItemWeapon original(0x2222);
    original.setStackSize(99);
    original.setQuantity(42);
    original.setReqLvl(75);
    original.setSkillType(SKILL_GREAT_SWORD);
    original.setSubSkillType(SUBSKILL_LONGBOW);
    original.setILvlSkill(242);
    original.setILvlParry(188);
    original.setILvlMacc(215);
    original.setDelay(240);
    original.setBaseDelay(300);
    original.setDamage(123);
    original.setDmgType(xi::DamageType::Slashing);
    original.setAdditionalEffect(7);
    original.setMaxHit(1);
    original.setDPS(12.75);
    original.setTotalUnlockPointsNeeded(300);
    original.setCurrentUnlockPoints(120);
    original.addModifier(Mod::DEF, 20);

    CItemWeapon copy(original);
    original.setQuantity(1);
    original.setReqLvl(1);
    original.setSkillType(SKILL_SWORD);
    original.setSubSkillType(SUBSKILL_GUN);
    original.setILvlSkill(1);
    original.setILvlParry(1);
    original.setILvlMacc(1);
    original.setDelay(60);
    original.setBaseDelay(60);
    original.setDamage(1);
    original.setDmgType(xi::DamageType::Piercing);
    original.setAdditionalEffect(1);
    original.setMaxHit(0);
    original.setDPS(1.0);
    original.setTotalUnlockPointsNeeded(1);
    original.setCurrentUnlockPoints(1);
    original.delModifier(Mod::DEF, 20);

    bool ok = true;
    ok      = expectUInt(copy.getID(), 0x2222, "copy id") && ok;
    ok      = expectBool(copy.isType(ITEM_USABLE), true, "copy usable type flag") && ok;
    ok      = expectBool(copy.isType(ITEM_EQUIPMENT), true, "copy equipment type flag") && ok;
    ok      = expectBool(copy.isType(ITEM_WEAPON), true, "copy weapon type flag") && ok;
    ok      = expectUInt(copy.getQuantity(), 42, "copy quantity") && ok;
    ok      = expectUInt(copy.getReqLvl(), 75, "copy required level") && ok;
    ok      = expectUInt(copy.getSkillType(), SKILL_GREAT_SWORD, "copy skill") && ok;
    ok      = expectBool(copy.isTwoHanded(), true, "copy two-handed") && ok;
    ok      = expectUInt(copy.getSubSkillType(), SUBSKILL_LONGBOW, "copy subskill") && ok;
    ok      = expectUInt(copy.getILvlSkill(), 242, "copy ilvl skill") && ok;
    ok      = expectUInt(copy.getILvlParry(), 188, "copy ilvl parry") && ok;
    ok      = expectUInt(copy.getILvlMacc(), 215, "copy ilvl macc") && ok;
    ok      = expectUInt(copy.getDelay(), 4000, "copy delay") && ok;
    ok      = expectUInt(copy.getBaseDelay(), 300, "copy base delay") && ok;
    ok      = expectUInt(copy.getDamage(), 123, "copy damage") && ok;
    ok      = expectUInt(static_cast<uint16>(copy.getDmgType()), static_cast<uint16>(xi::DamageType::Slashing), "copy damage type") && ok;
    ok      = expectUInt(copy.getAdditionalEffect(), 7, "copy additional effect") && ok;
    ok      = expectUInt(copy.getHitCount(), 1, "copy hit count") && ok;
    ok      = expectDouble(copy.getDPS(), 12.75, "copy dps") && ok;
    ok      = expectUInt(copy.getTotalUnlockPointsNeeded(), 300, "copy unlock total") && ok;
    ok      = expectUInt(copy.getCurrentUnlockPoints(), 120, "copy unlock current") && ok;
    ok      = expectBool(copy.isDirty(), true, "copy dirty flag") && ok;
    ok      = expectInt(copy.getModifier(Mod::DEF), 20, "copy modifier") && ok;
    return ok;
}

} // namespace

auto runItemWeaponSelfTests() -> bool
{
    bool ok = true;
    ok      = testConstructorDefaults() && ok;
    ok      = testScalarFieldsAndDelay() && ok;
    ok      = testSkillPredicates() && ok;
    ok      = testUnlockAndRodExdata() && ok;
    ok      = testHitCountBounds() && ok;
    ok      = testCopyConstructorCopiesFields() && ok;
    return ok;
}
