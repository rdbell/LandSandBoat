#include "test_ranged_acc_1642.h"

#include "map/ranged_acc_capacity.h"

#include <iostream>

namespace
{
using namespace rangedacchelpers;

auto Check() -> bool
{
    // Default pins
    if (DefaultRangedAGIAccuracyMultiplier != 0.75f || AutomatonAGIAccuracyMultiplier != 0.5f ||
        TrustAGIAccuracyMultiplier != 0.75f)
    {
        return false;
    }

    // --- GetAccFromSkill soft curves ---
    if (GetAccFromSkill(0) != 0 || GetAccFromSkill(100) != 100 || GetAccFromSkill(200) != 200)
    {
        return false;
    }
    if (GetAccFromSkill(201) != 200 || GetAccFromSkill(300) != 290 || GetAccFromSkill(400) != 380)
    {
        return false;
    }
    if (GetAccFromSkill(401) != 380 || GetAccFromSkill(500) != 460 || GetAccFromSkill(600) != 540)
    {
        return false;
    }
    if (GetAccFromSkill(601) != 540 || GetAccFromSkill(650) != 585 || GetAccFromSkill(700) != 630)
    {
        return false;
    }

    // --- Gates ---
    if (WeaknessBlocksRACC(false, 2) || WeaknessBlocksRACC(true, 1) || !WeaknessBlocksRACC(true, 2))
    {
        return false;
    }
    if (!PCRACCMissingAmmo(true, false, false) || PCRACCMissingAmmo(true, true, false) ||
        PCRACCMissingAmmo(true, false, true))
    {
        return false;
    }
    if (!PCRACCWeaponReject(false, false, false) || !PCRACCWeaponReject(true, true, false) ||
        !PCRACCWeaponReject(true, false, true) || PCRACCWeaponReject(true, false, false))
    {
        return false;
    }

    // --- Food quirk: (100 + FOOD * RACC) / 100, not RACC * FOOD / 100 ---
    if (FoodRACCBonus(100, 10, 1000) != 11 || FoodRACCBonus(100, 10, 5) != 5)
    {
        return false;
    }
    if (FoodRACCBonus(200, 0, 0) != 0 || FoodRACCBonus(200, 0, 100) != 1)
    {
        return false;
    }
    if (FoodRACCBonus(50, 1, 100) != 1)
    {
        return false;
    }
    if (FinalizeRACC(100, 10, 1000) != 111 || FinalizeRACC(-50, 0, 0) != 1)
    {
        return false;
    }

    // --- AGI floor ---
    if (FloorAGIContribution(100, 0.75f) != 75 || FloorAGIContribution(101, 0.75f) != 75 ||
        FloorAGIContribution(101, 0.5f) != 50)
    {
        return false;
    }

    // --- Full ResolveRACC cases (parity with Go rangedacc_test) ---

    // Weakness → 1 (RATT would return 0)
    {
        RACCParams p{};
        p.weaknessActive = true;
        p.weaknessPower  = 2;
        p.isPC           = true;
        p.hasWeapon      = true;
        p.weaponSkill    = 300;
        p.agi            = 100;
        if (ResolveRACC(p) != 1)
        {
            return false;
        }
    }

    // PC no ammo → 0
    {
        RACCParams p{};
        p.isPC            = true;
        p.hasRangedWeapon = true;
        p.skillIsThrowing = false;
        p.hasAmmoWeapon   = false;
        p.hasWeapon       = true;
        p.weaponSkill     = 300;
        if (ResolveRACC(p) != 0)
        {
            return false;
        }
    }

    // Throwing without ammo allowed → skill 100
    {
        RACCParams p{};
        p.isPC            = true;
        p.hasRangedWeapon = true;
        p.skillIsThrowing = true;
        p.hasAmmoWeapon   = false;
        p.hasWeapon       = true;
        p.weaponSkill     = 100;
        if (ResolveRACC(p) != 100)
        {
            return false;
        }
    }

    // PC weapon reject → 0
    {
        RACCParams p{};
        p.isPC      = true;
        p.hasWeapon = false;
        if (ResolveRACC(p) != 0)
        {
            return false;
        }
        p.hasWeapon   = true;
        p.dmgTypeNone = true;
        if (ResolveRACC(p) != 0)
        {
            return false;
        }
        p.dmgTypeNone = false;
        p.skillIsNone = true;
        if (ResolveRACC(p) != 0)
        {
            return false;
        }
    }

    // Food path: skill 100, FOOD 10 cap 1000 → 111
    {
        RACCParams p{};
        p.isPC        = true;
        p.hasWeapon   = true;
        p.weaponSkill = 100;
        p.foodRACCP   = 10;
        p.foodRACCCap = 1000;
        if (ResolveRACC(p) != 111)
        {
            return false;
        }
    }

    // PC default AGI mult: AGI 100 → 75
    {
        RACCParams p{};
        p.isPC                   = true;
        p.hasWeapon              = true;
        p.agi                    = 100;
        p.rangedAGIAccMultiplier = DefaultRangedAGIAccuracyMultiplier;
        if (ResolveRACC(p) != 75)
        {
            return false;
        }
    }

    // skill + ilvl: 200+50 → GetAcc(250)=245
    {
        RACCParams p{};
        p.isPC            = true;
        p.hasWeapon       = true;
        p.weaponSkill     = 200;
        p.weaponILvlSkill = 50;
        if (ResolveRACC(p) != 245)
        {
            return false;
        }
    }

    // Fishing ignores skill → max(1,0)=1
    {
        RACCParams p{};
        p.isPC          = true;
        p.hasWeapon     = true;
        p.skillIsFishing = true;
        p.weaponSkill   = 300;
        if (ResolveRACC(p) != 1)
        {
            return false;
        }
    }

    // Automaton: skill 300→290, AGI 50→25, ACC 10 + bonus 5 = 330
    {
        RACCParams p{};
        p.isPet                = true;
        p.isAutomaton          = true;
        p.automatonRangedSkill = 300;
        p.agi                  = 50;
        p.accMod               = 10;
        p.bonusAcc             = 5;
        if (ResolveRACC(p) != 330)
        {
            return false;
        }
    }

    // Trust max skill
    {
        RACCParams p{};
        p.isTrust           = true;
        p.archerySkill      = 100;
        p.marksmanshipSkill = 250;
        p.throwingSkill     = 50;
        p.raccMod           = 5;
        if (ResolveRACC(p) != 250)
        {
            return false;
        }
    }

    // Pet tandem + correlation inject
    {
        RACCParams p{};
        p.isPet                       = true;
        p.raccMod                     = 10;
        p.tandemActive                = true;
        p.masterIsPC                  = true;
        p.tandemStrikePower           = 15;
        p.monsterCorrelationAdvantage = true;
        p.enhancesMonsterCorrelation  = 20;
        p.agi                         = 40;
        if (ResolveRACC(p) != 65)
        {
            return false;
        }
        // no advantage
        p.monsterCorrelationAdvantage = false;
        // 10 + 15 + 20(agi/2) = 45 without corr
        if (ResolveRACC(p) != 45)
        {
            return false;
        }
    }

    // Mob: no correlation even if inject true
    {
        RACCParams p{};
        p.raccMod                     = 10;
        p.monsterCorrelationAdvantage = true;
        p.enhancesMonsterCorrelation  = 20;
        if (ResolveRACC(p) != 10)
        {
            return false;
        }
    }

    // Combined PC: 405
    {
        RACCParams p{};
        p.isPC                   = true;
        p.hasWeapon              = true;
        p.weaponSkill            = 300;
        p.raccMod                = 15;
        p.bonusAcc               = 5;
        p.rangedAccuracyBonuses  = 10;
        p.agi                    = 100;
        p.rangedAGIAccMultiplier = DefaultRangedAGIAccuracyMultiplier;
        p.foodRACCP              = 8;
        p.foodRACCCap            = 10;
        if (ResolveRACC(p) != 405)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runRangedAcc1642SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "ranged_acc_1642 self-tests failed\n";
        return false;
    }
    return true;
}
