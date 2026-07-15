#include "test_main_weapon_dmg_1647.h"

#include "map/main_weapon_dmg_capacity.h"

#include <iostream>

namespace
{
using namespace mainweapondmghelpers;

auto Check() -> bool
{
    // --- Mob: plain / mult / clamp ---
    if (ResolveMobMainWeaponDmg(50, 0, 100, 0, 0, 0) != 50)
    {
        return false;
    }
    // floor((50+10)*1.0) + 5 + 3 = 68
    if (ResolveMobMainWeaponDmg(50, 10, 100, 0, 5, 3) != 68)
    {
        return false;
    }
    // floor(40 * 1.5) = 60
    if (ResolveMobMainWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // baseDamageMultiplier overrides dmgMult: floor(40 * 2.0) = 80
    if (ResolveMobMainWeaponDmg(40, 0, 150, 200, 0, 0) != 80)
    {
        return false;
    }
    // baseMult 0 does not override
    if (ResolveMobMainWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // floor((10+5)*0.5) = floor(7.5) = 7
    if (ResolveMobMainWeaponDmg(10, 5, 50, 0, 0, 0) != 7)
    {
        return false;
    }
    // clamp low
    if (ResolveMobMainWeaponDmg(0, 0, 100, 0, 0, 0) != 1)
    {
        return false;
    }
    if (ResolveMobMainWeaponDmg(5, 0, 100, 0, -100, 0) != 1)
    {
        return false;
    }
    // clamp high
    if (ResolveMobMainWeaponDmg(40000, 30000, 200, 0, 1000, 0) != 65535)
    {
        return false;
    }
    // fixture: 50 + rating 10 + offset 6 = 66
    if (ResolveMobMainWeaponDmg(50, 0, 100, 0, 10, 6) != 66)
    {
        return false;
    }

    // --- Automaton (skill 87 → base 23) ---
    if (ResolveAutomatonMainWeaponDmg(87, 5) != 28)
    {
        return false;
    }
    if (ResolveAutomatonMainWeaponDmg(87, -2) != 21)
    {
        return false;
    }

    // --- Wyvern: floor(75/2)+3 = 40 + 5 ---
    if (ResolveWyvernMainWeaponDmg(75, 5) != 45)
    {
        return false;
    }
    if (ResolveWyvernMainWeaponDmg(50, 0) != 28) // 25+3
    {
        return false;
    }

    // --- Jug: lvl 30 → 32 (binary 1.4 trunc) + 2; lvl 40 → 40 ---
    if (ResolveJugMainWeaponDmg(30, 2) != 34)
    {
        return false;
    }
    if (ResolveJugMainWeaponDmg(40, 0) != 40)
    {
        return false;
    }

    // --- Avatar clamp ---
    if (ResolveAvatarMainWeaponDmg(40, 5) != 45)
    {
        return false;
    }
    if (ResolveAvatarMainWeaponDmg(0, 0) != 1)
    {
        return false;
    }
    if (ResolveAvatarMainWeaponDmg(65530, 100) != 65535)
    {
        return false;
    }
    if (ResolveAvatarMainWeaponDmg(10, -20) != 1)
    {
        return false;
    }

    // --- PC / other ---
    if (ResolvePCMainWeaponDmg(false, 99, 1, 75, true, 0, 0) != 0)
    {
        return false;
    }
    // 45 + 2 + 10 = 57
    if (ResolvePCMainWeaponDmg(true, 45, 50, 75, true, 2, 10) != 57)
    {
        return false;
    }
    // level-ok
    if (ResolvePCMainWeaponDmg(true, 50, 75, 75, true, 0, 5) != 55)
    {
        return false;
    }
    // underlevel: 100 * 50 * 3 / 4 / 75 = 50; +2 +5 = 57
    if (ResolvePCMainWeaponDmg(true, 100, 75, 50, true, 2, 5) != 57)
    {
        return false;
    }
    // 80 * 40 * 3 / 4 / 60 = 40
    if (ResolvePCMainWeaponDmg(true, 80, 60, 40, true, 0, 0) != 40)
    {
        return false;
    }
    // trunc: 99 * 10 * 3 / 4 / 50 = 14
    if (ResolvePCMainWeaponDmg(true, 99, 50, 10, true, 0, 0) != 14)
    {
        return false;
    }
    // non-PC underlevel: no scale
    if (ResolvePCMainWeaponDmg(true, 100, 75, 50, false, 0, 0) != 100)
    {
        return false;
    }
    if (ResolvePCMainWeaponDmg(true, 30, 1, 75, true, 3, 7) != 40)
    {
        return false;
    }
    if (ResolvePCMainWeaponDmg(true, 40, 0, 0, true, 0, 0) != 40)
    {
        return false;
    }
    if (ResolvePCMainWeaponDmg(true, 0, 1, 75, true, 4, 1) != 5)
    {
        return false;
    }

    // --- Dispatcher ---
    {
        Params p{};
        p.kind          = EntityKind::Mob;
        p.weaponDamage  = 50;
        p.dmgMult       = 100;
        p.mainDmgRating = 10;
        p.damageOffset  = 6;
        if (ResolveMainWeaponDmg(p) != 66)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind                = EntityKind::PetAutomaton;
        p.automatonMeleeSkill = 87;
        p.mainDmgRating       = 5;
        if (ResolveMainWeaponDmg(p) != 28)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind          = EntityKind::PetWyvern;
        p.mLevel        = 75;
        p.mainDmgRating = 5;
        if (ResolveMainWeaponDmg(p) != 45)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind          = EntityKind::PetAvatar;
        p.weaponDmg     = 40;
        p.mainDmgRating = 5;
        if (ResolveMainWeaponDmg(p) != 45)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind          = EntityKind::PetJug;
        p.mLevel        = 30;
        p.mainDmgRating = 2;
        if (ResolveMainWeaponDmg(p) != 34)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind          = EntityKind::PC;
        p.hasWeapon     = true;
        p.weaponDmg     = 100;
        p.reqLvl        = 75;
        p.mLevel        = 50;
        p.itemDmgRating = 2;
        p.mainDmgRating = 5;
        if (ResolveMainWeaponDmg(p) != 57)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind      = EntityKind::PC;
        p.hasWeapon = false;
        p.weaponDmg = 99;
        if (ResolveMainWeaponDmg(p) != 0)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind      = EntityKind::Other;
        p.hasWeapon = true;
        p.weaponDmg = 100;
        p.reqLvl    = 75;
        p.mLevel    = 50;
        if (ResolveMainWeaponDmg(p) != 100)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runMainWeaponDmg1647SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "main_weapon_dmg_1647 self-tests failed\n";
        return false;
    }
    return true;
}
