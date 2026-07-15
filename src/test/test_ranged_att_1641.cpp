#include "test_ranged_att_1641.h"

#include "map/ranged_att_capacity.h"

#include <iostream>

namespace
{
using namespace rangedatthelpers;

auto Check() -> bool
{
    // Constant pins
    if (SkillNone != 0 || SkillThrowing != 27 || SkillFishing != 48 || SkillArchery != 25 ||
        SkillMarksmanship != 26 || SkillAutomatonRanged != 23 || BaseRATT != 8)
    {
        return false;
    }

    // --- Weakness gate ---
    if (ShouldReturnZeroRATTWeakness(false, 99) || ShouldReturnZeroRATTWeakness(true, 1))
    {
        return false;
    }
    if (!ShouldReturnZeroRATTWeakness(true, 2) || !ShouldReturnZeroRATTWeakness(true, 5))
    {
        return false;
    }

    // --- PC no-ammo gate ---
    if (!ShouldReturnZeroPCNoAmmo(true, SkillArchery, false) ||
        ShouldReturnZeroPCNoAmmo(true, SkillThrowing, false) ||
        ShouldReturnZeroPCNoAmmo(true, SkillArchery, true))
    {
        return false;
    }

    // --- Non-damaging ---
    if (!ShouldReturnZeroNonDamagingWeapon(DamageTypeNone, SkillArchery) ||
        !ShouldReturnZeroNonDamagingWeapon(1, SkillNone) ||
        ShouldReturnZeroNonDamagingWeapon(1, SkillArchery))
    {
        return false;
    }

    // --- PC weapon skill resolution ---
    {
        PCRangedWeaponParams p{};
        p.hasRangedWeapon = true;
        p.rangedSkillType = SkillArchery;
        p.rangedDmgType   = 1;
        p.rangedGetSkill  = 200;
        p.hasAmmoWeapon   = false;
        auto r            = ResolvePCRangedWeaponSkillLevel(p);
        if (!r.returnZero || r.skillLevel != 0)
        {
            return false;
        }
    }
    {
        PCRangedWeaponParams p{};
        auto r = ResolvePCRangedWeaponSkillLevel(p);
        if (!r.returnZero)
        {
            return false;
        }
    }
    {
        PCRangedWeaponParams p{};
        p.hasRangedWeapon  = true;
        p.rangedSkillType  = SkillArchery;
        p.rangedDmgType    = 1;
        p.rangedGetSkill   = 200;
        p.rangedILvlSkill  = 30;
        p.hasAmmoWeapon    = true;
        auto r             = ResolvePCRangedWeaponSkillLevel(p);
        if (r.returnZero || r.skillLevel != 230)
        {
            return false;
        }
    }
    {
        // ammo-only throwing
        PCRangedWeaponParams p{};
        p.hasAmmoWeapon = true;
        p.ammoSkillType = SkillThrowing;
        p.ammoDmgType   = 1;
        p.ammoGetSkill  = 150;
        p.ammoILvlSkill = 10;
        auto r          = ResolvePCRangedWeaponSkillLevel(p);
        if (r.returnZero || r.skillLevel != 160)
        {
            return false;
        }
    }
    {
        // fishing: skill stays 0
        PCRangedWeaponParams p{};
        p.hasRangedWeapon = true;
        p.rangedSkillType = SkillFishing;
        p.rangedDmgType   = 1;
        p.rangedGetSkill  = 999;
        p.hasAmmoWeapon   = true;
        auto r            = ResolvePCRangedWeaponSkillLevel(p);
        if (r.returnZero || r.skillLevel != 0)
        {
            return false;
        }
    }

    // --- Entity skill / STR mult ---
    if (MaxRangedSkill(10, 30, 20) != 30)
    {
        return false;
    }
    {
        auto r = ResolveEntitySkillAndSTRMultiplier(true, false, false, 200, 1.0, 0, 0, 0, 0);
        if (r.skillLevel != 200 || r.strMultiplier != 1.0)
        {
            return false;
        }
        r = ResolveEntitySkillAndSTRMultiplier(false, true, false, 0, 1.0, 180, 0, 0, 0);
        if (r.skillLevel != 180 || r.strMultiplier != 0.5)
        {
            return false;
        }
        r = ResolveEntitySkillAndSTRMultiplier(false, false, true, 0, 1.0, 0, 100, 250, 90);
        if (r.skillLevel != 250 || r.strMultiplier != 0.75)
        {
            return false;
        }
        r = ResolveEntitySkillAndSTRMultiplier(false, false, false, 0, 1.0, 0, 0, 0, 0);
        if (r.skillLevel != 0 || r.strMultiplier != 0.5)
        {
            return false;
        }
    }

    // --- Floor STR ---
    if (FloorSTRContribution(100, 0.5) != 50 || FloorSTRContribution(100, 0.75) != 75)
    {
        return false;
    }
    if (FloorSTRContribution(99, 0.75) != 74 || FloorSTRContribution(1, 0.5) != 0)
    {
        return false;
    }

    // --- Food / finalize float path ---
    if (FoodRATTBonus(100, 50, 20) != 20 || FoodRATTBonus(100, 50, 1000) != 50)
    {
        return false;
    }
    if (FoodRATTBonus(33, 50, 1000) != 16 || FoodRATTBonus(50, 1, 100) != 0)
    {
        return false;
    }
    if (FinalizeRATT(100, 0, 50, 20) != 120 || FinalizeRATT(100, 33, 0, 0) != 133)
    {
        return false;
    }
    if (FinalizeRATT(50, 1, 0, 0) != 50 || FinalizeRATT(8, -200, 0, 0) != 1)
    {
        return false;
    }

    // --- Full ResolveRATT cases (parity with Go rangedatt_test) ---

    // Weakness zero
    {
        RATTParams p{};
        p.returnZero    = true;
        p.str           = 100;
        p.strMultiplier = 1.0;
        p.skillLevel    = 200;
        if (ResolveRATT(p) != 0)
        {
            return false;
        }
    }

    // Automaton skill 150, STR 50 * 0.5 → 183
    {
        auto ent = ResolveEntitySkillAndSTRMultiplier(false, true, false, 0, 1.0, 150, 0, 0, 0);
        RATTParams p{};
        p.skillLevel    = ent.skillLevel;
        p.str           = 50;
        p.strMultiplier = ent.strMultiplier;
        if (ResolveRATT(p) != 183)
        {
            return false;
        }
    }

    // Trust max skill 250, STR 100 * 0.75 → 333
    {
        auto ent = ResolveEntitySkillAndSTRMultiplier(false, false, true, 0, 1.0, 0, 100, 250, 90);
        RATTParams p{};
        p.skillLevel    = ent.skillLevel;
        p.str           = 100;
        p.strMultiplier = ent.strMultiplier;
        if (ResolveRATT(p) != 333)
        {
            return false;
        }
    }

    // Food ATTP-style cap: RATT=100, 50% cap 20 → 120
    {
        RATTParams p{};
        p.rattMod       = 92;
        p.strMultiplier = 0.5;
        p.foodRATTP     = 50;
        p.foodRATTCap   = 20;
        if (ResolveRATT(p) != 120)
        {
            return false;
        }
    }

    // Floor STR: 99 * 0.75 → 82
    {
        RATTParams p{};
        p.str           = 99;
        p.strMultiplier = 0.75;
        if (ResolveRATT(p) != 82)
        {
            return false;
        }
    }

    // Mob baked Mod::RATT: 8+200+40 = 248
    {
        RATTParams p{};
        p.rattMod       = 200;
        p.str           = 80;
        p.strMultiplier = DefaultNonPCSTRMultiplier;
        if (ResolveRATT(p) != 248)
        {
            return false;
        }
    }

    // bonusAtt + rangedAttackBonuses inject
    {
        RATTParams p{};
        p.bonusAtt             = 15;
        p.rattMod              = 10;
        p.skillLevel           = 200;
        p.rangedAttackBonuses  = 20;
        p.str                  = 100;
        p.strMultiplier        = 1.0;
        if (ResolveRATT(p) != 353)
        {
            return false;
        }
    }

    // Float RATTP half + floor
    {
        RATTParams p{};
        p.rattMod       = 42;
        p.rattP         = 1;
        p.strMultiplier = 0.5;
        if (ResolveRATT(p) != 50)
        {
            return false;
        }
        RATTParams q{};
        q.rattP         = -200;
        q.strMultiplier = 0.5;
        if (ResolveRATT(q) != 1)
        {
            return false;
        }
    }

    // Combined PC archery: skill 280, STR 100, RATTP 10, food 10/40 → 464
    {
        PCRangedWeaponParams wp{};
        wp.hasRangedWeapon = true;
        wp.rangedSkillType = SkillArchery;
        wp.rangedDmgType   = 1;
        wp.rangedGetSkill  = 250;
        wp.rangedILvlSkill = 30;
        wp.hasAmmoWeapon   = true;
        auto sk            = ResolvePCRangedWeaponSkillLevel(wp);
        if (sk.returnZero || sk.skillLevel != 280)
        {
            return false;
        }
        auto ent = ResolveEntitySkillAndSTRMultiplier(true, false, false, sk.skillLevel,
                                                      DefaultRangedSTRAttackMultiplier, 0, 0, 0, 0);
        RATTParams p{};
        p.skillLevel    = ent.skillLevel;
        p.str           = 100;
        p.strMultiplier = ent.strMultiplier;
        p.rattP         = 10;
        p.foodRATTP     = 10;
        p.foodRATTCap   = 40;
        if (ResolveRATT(p) != 464)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runRangedAtt1641SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ranged_att_1641 self-tests failed\n";
        return false;
    }
    return true;
}
