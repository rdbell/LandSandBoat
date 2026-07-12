#include "test_avatar_combat_1604.h"

#include "map/avatar_stats_capacity.h"

#include <iostream>

namespace
{
using namespace avatarstatshelpers;

auto Check() -> bool
{
    if (WeaponDamage(1) != 3 || WeaponDamage(75) != 77 || WeaponDamage(99) != 101)
    {
        return false;
    }
    if (SkillCapLevel(50) != 50 || SkillCapLevel(99) != 99 || SkillCapLevel(100) != 99)
    {
        return false;
    }
    if (MagicAttack(9) != 0 || MagicAttack(10) != 20 || MagicAttack(30) != 24 || MagicAttack(50) != 28 || MagicAttack(70) != 32)
    {
        return false;
    }
    if (AttackFromSkill(100) != 200 || SummonPhysAtkBonus(5) != 10 || SummonMagicDmgBonus(4) != 20 || BloodPactDmgBonus(3) != 9)
    {
        return false;
    }
    if (PhysicalDamageTaken != -5000 || CritDamageIncrease != 8 || RangedBaseDelay != 360)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAvatarCombat1604SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "avatar_combat_1604 self-tests failed\n";
        return false;
    }
    return true;
}
