#include "test_wyvern_stats_1605.h"

#include "map/pet_weapon_damage_capacity.h"
#include "map/wyvern_stats_capacity.h"

#include <iostream>

namespace
{
using namespace wyvernstatshelpers;

auto Check() -> bool
{
    if (ILvlBonus(0) != 0 || ILvlBonus(99) != 0 || ILvlBonus(100) != 1 || ILvlBonus(119) != 20 || ILvlBonus(130) != 20)
    {
        return false;
    }
    if (PetMainLevel(75, 5, 2) != 82 || PetMainLevel(99, 20, 0) != 119 || PetMainLevel(1, 0, -5) != 0)
    {
        return false;
    }
    if (SkillCapLevel(50) != 50 || SkillCapLevel(99) != 99 || SkillCapLevel(100) != 99)
    {
        return false;
    }
    if (MaxHPJobPointBonus(0) != 0 || MaxHPJobPointBonus(5) != 50)
    {
        return false;
    }
    if (WeaponDelay != 320 || DamageTaken != -4000 || SubtleBlow != 40 || CanParry != 1)
    {
        return false;
    }
    // Production setDamage uses master mLvl with WyvernWeaponDamage.
    if (petweapondamagehelpers::WyvernWeaponDamage(75) != 40 || petweapondamagehelpers::WyvernWeaponDamage(1) != 3)
    {
        return false;
    }
    return true;
}
} // namespace

auto runWyvernStats1605SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "wyvern_stats_1605 self-tests failed\n";
        return false;
    }
    return true;
}
