#include "test_ranged_weapon_dmg_1650.h"

#include "map/ranged_weapon_dmg_capacity.h"

#include <iostream>

namespace
{
using namespace rangedweapondmghelpers;

auto Check() -> bool
{
    // --- Mob: plain / mult / clamp ---
    if (ResolveMobRangedWeaponDmg(50, 0, 100, 0, 0, 0) != 50)
    {
        return false;
    }
    // floor((50+10)*1.0) + 5 + 3 = 68
    if (ResolveMobRangedWeaponDmg(50, 10, 100, 0, 5, 3) != 68)
    {
        return false;
    }
    // floor(40 * 1.5) = 60
    if (ResolveMobRangedWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // baseDamageMultiplier overrides dmgMult: floor(40 * 2.0) = 80
    if (ResolveMobRangedWeaponDmg(40, 0, 150, 200, 0, 0) != 80)
    {
        return false;
    }
    // baseMult 0 does not override
    if (ResolveMobRangedWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // floor((10+5)*0.5) = floor(7.5) = 7
    if (ResolveMobRangedWeaponDmg(10, 5, 50, 0, 0, 0) != 7)
    {
        return false;
    }
    // clamp low
    if (ResolveMobRangedWeaponDmg(0, 0, 100, 0, 0, 0) != 1)
    {
        return false;
    }
    if (ResolveMobRangedWeaponDmg(5, 0, 100, 0, -100, 0) != 1)
    {
        return false;
    }
    // clamp high
    if (ResolveMobRangedWeaponDmg(40000, 30000, 200, 0, 1000, 0) != 65535)
    {
        return false;
    }
    // fixture: 50 + rating 10 + ranged offset 6 = 66
    if (ResolveMobRangedWeaponDmg(50, 0, 100, 0, 10, 6) != 66)
    {
        return false;
    }

    // --- Automaton (skill 87 → base 23) ---
    if (ResolveAutomatonRangedWeaponDmg(87, 5) != 28)
    {
        return false;
    }
    if (ResolveAutomatonRangedWeaponDmg(87, -2) != 21)
    {
        return false;
    }

    // --- Wyvern: floor(75/2)+3 = 40 + 5 ---
    if (ResolveWyvernRangedWeaponDmg(75, 5) != 45)
    {
        return false;
    }
    if (ResolveWyvernRangedWeaponDmg(50, 0) != 28) // 25+3
    {
        return false;
    }

    // --- Jug: lvl 30 → 32 (binary 1.4 trunc) + 2; lvl 40 → 40 ---
    if (ResolveJugRangedWeaponDmg(30, 2) != 34)
    {
        return false;
    }
    if (ResolveJugRangedWeaponDmg(40, 0) != 40)
    {
        return false;
    }

    // --- Avatar clamp ---
    if (ResolveAvatarRangedWeaponDmg(40, 5) != 45)
    {
        return false;
    }
    if (ResolveAvatarRangedWeaponDmg(0, 0) != 1)
    {
        return false;
    }
    if (ResolveAvatarRangedWeaponDmg(65530, 100) != 65535)
    {
        return false;
    }
    if (ResolveAvatarRangedWeaponDmg(10, -20) != 1)
    {
        return false;
    }

    // --- PC / other ---
    // empty slots: rating only
    if (ResolvePCRangedWeaponDmg(false, 99, 1, 0, false, 0, 0, 0, 75, true, 0) != 0)
    {
        return false;
    }
    if (ResolvePCRangedWeaponDmg(false, 0, 0, 0, false, 0, 0, 0, 75, true, 7) != 7)
    {
        return false;
    }
    // ranged only: 45 + 2 + 10 = 57
    if (ResolvePCRangedWeaponDmg(true, 45, 50, 2, false, 0, 0, 0, 75, true, 10) != 57)
    {
        return false;
    }
    // ammo only: 20 + 1 + 3 = 24
    if (ResolvePCRangedWeaponDmg(false, 0, 0, 0, true, 20, 1, 1, 75, true, 3) != 24)
    {
        return false;
    }
    // both: 40+2 + 10+1 + 5 = 58
    if (ResolvePCRangedWeaponDmg(true, 40, 1, 2, true, 10, 1, 1, 75, true, 5) != 58)
    {
        return false;
    }
    // underlevel ranged: 100 * 50 * 3 / 4 / 75 = 50; +2 + ammo 10+1 + rating 5 = 68
    if (ResolvePCRangedWeaponDmg(true, 100, 75, 2, true, 10, 1, 1, 50, true, 5) != 68)
    {
        return false;
    }
    // ammo underlevel: 80 * 40 * 3 / 4 / 60 = 40
    if (ResolvePCRangedWeaponDmg(false, 0, 0, 0, true, 80, 60, 0, 40, true, 0) != 40)
    {
        return false;
    }
    // trunc: 99 * 10 * 3 / 4 / 50 = 14
    if (ResolvePCRangedWeaponDmg(true, 99, 50, 0, false, 0, 0, 0, 10, true, 0) != 14)
    {
        return false;
    }
    // non-PC underlevel: no scale
    if (ResolvePCRangedWeaponDmg(true, 100, 75, 0, false, 0, 0, 0, 50, false, 0) != 100)
    {
        return false;
    }
    // reqLvl == mLevel
    if (ResolvePCRangedWeaponDmg(true, 50, 75, 0, false, 0, 0, 0, 75, true, 5) != 55)
    {
        return false;
    }
    // both underlevel independently: 50 + 1 + 30 + 2 + 3 = 86
    if (ResolvePCRangedWeaponDmg(true, 100, 75, 1, true, 60, 75, 2, 50, true, 3) != 86)
    {
        return false;
    }

    // --- Dispatcher ---
    {
        Params p{};
        p.kind               = EntityKind::Mob;
        p.weaponDamage       = 50;
        p.dmgMult            = 100;
        p.rangedDmgRating    = 10;
        p.rangedDamageOffset = 6;
        if (ResolveRangedWeaponDmg(p) != 66)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind                 = EntityKind::PetAutomaton;
        p.automatonRangedSkill = 87;
        p.rangedDmgRating      = 5;
        if (ResolveRangedWeaponDmg(p) != 28)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind            = EntityKind::PetWyvern;
        p.mLevel          = 75;
        p.rangedDmgRating = 5;
        if (ResolveRangedWeaponDmg(p) != 45)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind            = EntityKind::PetAvatar;
        p.rangedWeaponDmg = 40;
        p.rangedDmgRating = 5;
        if (ResolveRangedWeaponDmg(p) != 45)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind            = EntityKind::PetJug;
        p.mLevel          = 30;
        p.rangedDmgRating = 2;
        if (ResolveRangedWeaponDmg(p) != 34)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind                = EntityKind::PC;
        p.hasRanged           = true;
        p.rangedWeaponDmg     = 100;
        p.rangedReqLvl        = 75;
        p.rangedItemDmgRating = 2;
        p.hasAmmo             = true;
        p.ammoDmg             = 10;
        p.ammoItemDmgRating   = 1;
        p.mLevel              = 50;
        p.rangedDmgRating     = 5;
        if (ResolveRangedWeaponDmg(p) != 68)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind            = EntityKind::PC;
        p.rangedDmgRating = 9;
        if (ResolveRangedWeaponDmg(p) != 9)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind            = EntityKind::Other;
        p.hasRanged       = true;
        p.rangedWeaponDmg = 100;
        p.rangedReqLvl    = 75;
        p.mLevel          = 50;
        if (ResolveRangedWeaponDmg(p) != 100)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runRangedWeaponDmg1650SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ranged_weapon_dmg_1650 self-tests failed\n";
        return false;
    }
    return true;
}
