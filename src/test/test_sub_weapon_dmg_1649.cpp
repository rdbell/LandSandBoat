#include "test_sub_weapon_dmg_1649.h"

#include "map/sub_weapon_dmg_capacity.h"

#include <iostream>

namespace
{
using namespace subweapondmghelpers;

auto Check() -> bool
{
    // --- Mob / non-automaton pet: plain / mult / clamp ---
    if (ResolveMobOrPetSubWeaponDmg(50, 0, 100, 0, 0, 0) != 50)
    {
        return false;
    }
    // floor((50+10)*1.0) + 5 + 3 = 68
    if (ResolveMobOrPetSubWeaponDmg(50, 10, 100, 0, 5, 3) != 68)
    {
        return false;
    }
    // floor(40 * 1.5) = 60
    if (ResolveMobOrPetSubWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // baseDamageMultiplier overrides dmgMult: floor(40 * 2.0) = 80
    if (ResolveMobOrPetSubWeaponDmg(40, 0, 150, 200, 0, 0) != 80)
    {
        return false;
    }
    // baseMult 0 does not override
    if (ResolveMobOrPetSubWeaponDmg(40, 0, 150, 0, 0, 0) != 60)
    {
        return false;
    }
    // floor((10+5)*0.5) = floor(7.5) = 7
    if (ResolveMobOrPetSubWeaponDmg(10, 5, 50, 0, 0, 0) != 7)
    {
        return false;
    }
    // clamp low (missing sub inject weaponDamage=0)
    if (ResolveMobOrPetSubWeaponDmg(0, 0, 100, 0, 0, 0) != 1)
    {
        return false;
    }
    if (ResolveMobOrPetSubWeaponDmg(5, 0, 100, 0, -100, 0) != 1)
    {
        return false;
    }
    // clamp high
    if (ResolveMobOrPetSubWeaponDmg(40000, 30000, 200, 0, 1000, 0) != 65535)
    {
        return false;
    }
    // fixture: 50 + rating 10 + offset 6 = 66
    if (ResolveMobOrPetSubWeaponDmg(50, 0, 100, 0, 10, 6) != 66)
    {
        return false;
    }
    // non-mob pet inject: mult 100, zero mods
    if (ResolveMobOrPetSubWeaponDmg(25, 0, 100, 0, 4, 0) != 29)
    {
        return false;
    }
    // missing sub + rating
    if (ResolveMobOrPetSubWeaponDmg(0, 0, 100, 0, 7, 0) != 7)
    {
        return false;
    }

    // --- PC / other ---
    if (ResolvePCSubWeaponDmg(false, 99, 1, 75, true, 0, 0) != 0)
    {
        return false;
    }
    // 45 + 2 + 10 = 57
    if (ResolvePCSubWeaponDmg(true, 45, 50, 75, true, 2, 10) != 57)
    {
        return false;
    }
    // level-ok
    if (ResolvePCSubWeaponDmg(true, 50, 75, 75, true, 0, 5) != 55)
    {
        return false;
    }
    // underlevel: 100 * 50 * 3 / 4 / 75 = 50; +2 +5 = 57
    if (ResolvePCSubWeaponDmg(true, 100, 75, 50, true, 2, 5) != 57)
    {
        return false;
    }
    // 80 * 40 * 3 / 4 / 60 = 40
    if (ResolvePCSubWeaponDmg(true, 80, 60, 40, true, 0, 0) != 40)
    {
        return false;
    }
    // trunc: 99 * 10 * 3 / 4 / 50 = 14
    if (ResolvePCSubWeaponDmg(true, 99, 50, 10, true, 0, 0) != 14)
    {
        return false;
    }
    // non-PC underlevel: no scale (automaton / trust)
    if (ResolvePCSubWeaponDmg(true, 100, 75, 50, false, 0, 0) != 100)
    {
        return false;
    }
    if (ResolvePCSubWeaponDmg(true, 30, 1, 75, true, 3, 7) != 40)
    {
        return false;
    }
    if (ResolvePCSubWeaponDmg(true, 40, 0, 0, true, 0, 0) != 40)
    {
        return false;
    }
    if (ResolvePCSubWeaponDmg(true, 0, 1, 75, true, 4, 1) != 5)
    {
        return false;
    }
    // automaton-style with ratings
    if (ResolvePCSubWeaponDmg(true, 12, 99, 1, false, 1, 2) != 15)
    {
        return false;
    }

    // --- Dispatcher ---
    {
        Params p{};
        p.kind         = EntityKind::MobOrPetNonAutomaton;
        p.weaponDamage = 50;
        p.dmgMult      = 100;
        p.subDmgRating = 10;
        p.damageOffset = 6;
        if (ResolveSubWeaponDmg(p) != 66)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind         = EntityKind::MobOrPetNonAutomaton;
        p.weaponDamage = 25;
        p.dmgMult      = 100;
        p.subDmgRating = 4;
        if (ResolveSubWeaponDmg(p) != 29)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind         = EntityKind::MobOrPetNonAutomaton;
        p.weaponDamage = 0;
        p.dmgMult      = 100;
        if (ResolveSubWeaponDmg(p) != 1)
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
        p.subDmgRating  = 5;
        if (ResolveSubWeaponDmg(p) != 57)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind      = EntityKind::PC;
        p.hasWeapon = false;
        p.weaponDmg = 99;
        if (ResolveSubWeaponDmg(p) != 0)
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
        if (ResolveSubWeaponDmg(p) != 100)
        {
            return false;
        }
    }
    {
        Params p{};
        p.kind      = EntityKind::Other;
        p.hasWeapon = false;
        if (ResolveSubWeaponDmg(p) != 0)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runSubWeaponDmg1649SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "sub_weapon_dmg_1649 self-tests failed\n";
        return false;
    }
    return true;
}
