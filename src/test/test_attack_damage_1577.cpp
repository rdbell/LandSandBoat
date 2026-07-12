#include "test_attack_damage_1577.h"

#include "map/attack_capacity.h"

#include <iostream>

namespace
{
using namespace attackhelpers;

auto Check() -> bool
{
    // Main hand baseline: 50+5=55 * 1.5 → 82
    {
        AttackDamageParams p{};
        p.slot        = SlotMain;
        p.weaponDmg   = 50;
        p.fSTR        = 5;
        p.damageRatio = 1.5f;
        p.scarletMult = 1.0f;
        if (CalculateAttackDamage(p) != 82)
        {
            return false;
        }
    }

    // SA bonus: DEX 100 → +100; 50+100=150
    {
        AttackDamageParams p{};
        p.isSneakAttack = true;
        p.dex           = 100;
        p.slot          = SlotMain;
        p.weaponDmg     = 50;
        p.damageRatio   = 1.0f;
        p.scarletMult   = 1.0f;
        if (CalculateAttackDamage(p) != 150)
        {
            return false;
        }
    }

    // DA additive quirk: floor(100+1.5)=101
    {
        AttackDamageParams p{};
        p.slot            = SlotMain;
        p.weaponDmg       = 100;
        p.damageRatio     = 1.0f;
        p.scarletMult     = 1.0f;
        p.attackType      = AttackTypeDouble;
        p.isPC            = true;
        p.doubleAttackDmg = 150;
        if (CalculateAttackDamage(p) != 101)
        {
            return false;
        }
    }

    // Ammo ignores SA bonus
    {
        AttackDamageParams p{};
        p.isSneakAttack = true;
        p.dex           = 100;
        p.slot          = SlotAmmo;
        p.weaponDmg     = 50;
        p.fSTR          = 5;
        p.damageRatio   = 1.0f;
        p.scarletMult   = 1.0f;
        if (CalculateAttackDamage(p) != 55)
        {
            return false;
        }
    }

    // Negative fSTR clamp
    {
        AttackDamageParams p{};
        p.slot        = SlotMain;
        p.weaponDmg   = 5;
        p.fSTR        = -20;
        p.damageRatio = 1.0f;
        p.scarletMult = 1.0f;
        if (CalculateAttackDamage(p) != 0)
        {
            return false;
        }
    }

    // Natural H2H
    if (NaturalH2HDamage(100) != 14)
    {
        return false;
    }

    return true;
}
} // namespace

auto runAttackDamage1577SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "attack_damage_1577 self-tests failed\n";
        return false;
    }
    return true;
}
