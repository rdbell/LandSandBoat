#include "test_weapon_skill_points_6851.h"

#include "map/entities/char_entity.h"
#include "map/items/item_weapon.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{
auto Check() -> bool
{
    auto character = CCharEntity{};

    if (charutils::AddWeaponSkillPoints(&character, SLOT_MAIN, 100))
    {
        return false;
    }

    auto inert = CItemWeapon{ 0x6851 };
    character.m_Weapons[SLOT_MAIN] = &inert;
    if (charutils::AddWeaponSkillPoints(&character, SLOT_MAIN, 100) || inert.getCurrentUnlockPoints() != 0)
    {
        return false;
    }

    auto weapon = CItemWeapon{ 0x6852 };
    weapon.setSkillType(SKILL_SWORD);
    weapon.setTotalUnlockPointsNeeded(300);
    character.m_Weapons[SLOT_MAIN] = &weapon;
    if (!charutils::AddWeaponSkillPoints(&character, SLOT_MAIN, 120) || weapon.getCurrentUnlockPoints() != 120 || weapon.isUnlocked())
    {
        return false;
    }
    // Completing the unlock emits client packets, whose construction needs the
    // database-backed test application. test_item_weapon already pins that
    // cap; this early self-test covers AddWeaponSkillPoints' eligible path.
    weapon.setCurrentUnlockPoints(300);
    return !charutils::AddWeaponSkillPoints(&character, SLOT_MAIN, 1) && weapon.getCurrentUnlockPoints() == 300;
}
} // namespace

auto runWeaponSkillPoints6851SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "weapon skill points 6851 self-test failed\n";
    }
    return ok;
}
