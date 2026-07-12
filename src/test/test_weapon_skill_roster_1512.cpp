#include "test_weapon_skill_roster_1512.h"

#include "map/weapon_skill_roster_capacity.h"

#include <iostream>

namespace
{
using weaponskillrosterhelpers::IsMatchingWeaponSkill;
using weaponskillrosterhelpers::IsSubEmptyOrNonEquipment;
using weaponskillrosterhelpers::MeleeSkillTypeFromMain;
using weaponskillrosterhelpers::RangedSkillTypeFromItem;
using weaponskillrosterhelpers::RealSkillLevels;
using weaponskillrosterhelpers::ShouldAddMeleeWeaponSkill;
using weaponskillrosterhelpers::ShouldAddRangedWeaponSkill;
using weaponskillrosterhelpers::ShouldConsiderRangedWeaponSkills;
using weaponskillrosterhelpers::ShouldSkipCheckWeaponSkill;
using weaponskillrosterhelpers::ShouldUnlockWeaponSkillOnSkillUp;
using weaponskillrosterhelpers::ShouldUseUnarmedH2H;
using weaponskillrosterhelpers::ShouldUseUnlockableWeaponMod;
using weaponskillrosterhelpers::SkillHandToHand;
using weaponskillrosterhelpers::SkillThrowing;
using weaponskillrosterhelpers::UnarmedLookMain;

auto Check() -> bool
{
    if (!IsMatchingWeaponSkill(true, 5, 5) || IsMatchingWeaponSkill(false, 5, 5) || IsMatchingWeaponSkill(true, 4, 5))
    {
        return false;
    }
    if (!ShouldSkipCheckWeaponSkill(false, false) || ShouldSkipCheckWeaponSkill(true, false) || ShouldSkipCheckWeaponSkill(false, true))
    {
        return false;
    }
    if (RealSkillLevels(255) != 25)
    {
        return false;
    }
    if (!ShouldUnlockWeaponSkillOnSkillUp(100, 100, true) || ShouldUnlockWeaponSkillOnSkillUp(100, 99, true) ||
        ShouldUnlockWeaponSkillOnSkillUp(100, 100, false))
    {
        return false;
    }
    if (!ShouldUseUnlockableWeaponMod(true, false, false) || !ShouldUseUnlockableWeaponMod(true, true, true) ||
        ShouldUseUnlockableWeaponMod(true, true, false) || ShouldUseUnlockableWeaponMod(false, false, false))
    {
        return false;
    }
    if (MeleeSkillTypeFromMain(false, 5) != SkillHandToHand || MeleeSkillTypeFromMain(true, 3) != 3)
    {
        return false;
    }
    if (!ShouldAddMeleeWeaponSkill(true, 10, 0) || !ShouldAddMeleeWeaponSkill(false, 42, 42) || ShouldAddMeleeWeaponSkill(false, 10, 0))
    {
        return false;
    }
    if (!ShouldConsiderRangedWeaponSkills(true, true, 26) || ShouldConsiderRangedWeaponSkills(true, true, SkillThrowing) ||
        ShouldConsiderRangedWeaponSkills(false, true, 26) || ShouldConsiderRangedWeaponSkills(true, false, 26))
    {
        return false;
    }
    if (RangedSkillTypeFromItem(true, 26) != 26 || RangedSkillTypeFromItem(false, 26) != 0)
    {
        return false;
    }
    if (!ShouldAddRangedWeaponSkill(true, 1, 0) || !ShouldAddRangedWeaponSkill(false, 7, 7) || ShouldAddRangedWeaponSkill(false, 1, 0))
    {
        return false;
    }
    if (!IsSubEmptyOrNonEquipment(false, true) || !IsSubEmptyOrNonEquipment(true, false) || IsSubEmptyOrNonEquipment(true, true))
    {
        return false;
    }
    if (!ShouldUseUnarmedH2H(true, false, true) || !ShouldUseUnarmedH2H(false, true, true) || ShouldUseUnarmedH2H(true, true, false) ||
        ShouldUseUnarmedH2H(false, false, true))
    {
        return false;
    }
    if (UnarmedLookMain(true) != 21 || UnarmedLookMain(false) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runWeaponSkillRoster1512SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "weapon skill roster 1512 self-test failed\n";
    }
    return ok;
}
