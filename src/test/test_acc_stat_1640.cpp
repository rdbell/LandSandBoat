#include "test_acc_stat_1640.h"

#include "map/acc_stat_capacity.h"

#include <iostream>

namespace
{
using namespace accstathelpers;

auto Check() -> bool
{
    // --- Default pins ---
    if (DefaultTwoHandedDEXAccuracyMultiplier != 0.75f ||
        DefaultHandToHandDEXAccuracyMultiplier != 0.75f ||
        DefaultOneHandMainDEXAccuracyMultiplier != 0.75f ||
        DefaultOneHandOffDEXAccuracyMultiplier != 0.75f)
    {
        return false;
    }
    if (DefaultAutomatonDEXMultiplier != 0.5f || DefaultUnsetDEXMultiplier != 0.5f)
    {
        return false;
    }

    // --- GetAccFromSkill curve ---
    if (GetAccFromSkill(0) != 0 || GetAccFromSkill(200) != 200)
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
    if (GetAccFromSkill(601) != 540 || GetAccFromSkill(700) != 630 || GetAccFromSkill(1000) != 900)
    {
        return false;
    }

    // --- DEX multiplier by attackNumber ---
    // attack 0 one-hand main
    if (ResolveAttackDEXMultiplier(0, true, false, false, false, false, false, false, 0.75f, 0.75f, 0.75f) != 0.75f)
    {
        return false;
    }
    // attack 0 H2H custom
    if (ResolveAttackDEXMultiplier(0, true, true, false, false, false, false, false, 0.5f, 0.5f, 0.9f) != 0.9f)
    {
        return false;
    }
    // attack 0 skill NONE + H2H skill
    if (ResolveAttackDEXMultiplier(0, true, false, true, true, false, false, false, 0.5f, 0.5f, 0.9f) != 0.9f)
    {
        return false;
    }
    // attack 1 sub off-hand
    if (ResolveAttackDEXMultiplier(1, true, false, false, false, true, false, false, 0.75f, 0.6f, 0.9f) != 0.6f)
    {
        return false;
    }
    // attack 1 sub H2H pair
    if (ResolveAttackDEXMultiplier(1, true, false, false, true, true, true, true, 0.75f, 0.6f, 0.9f) != 0.9f)
    {
        return false;
    }
    // attack 1 no sub, main H2H
    if (ResolveAttackDEXMultiplier(1, true, true, false, false, false, false, false, 0.75f, 0.6f, 0.9f) != 0.9f)
    {
        return false;
    }
    // attack 1 no sub not H2H → 0.5
    if (ResolveAttackDEXMultiplier(1, true, false, false, false, false, false, false, 0.75f, 0.6f, 0.9f) != 0.5f)
    {
        return false;
    }
    // attack 2 always H2H
    if (ResolveAttackDEXMultiplier(2, false, false, false, false, false, false, false, 0.75f, 0.6f, 0.9f) != 0.9f)
    {
        return false;
    }

    // --- DEX float floor ---
    if (TruncDEXContribution(100, 0.75f) != 75 || TruncDEXContribution(99, 0.75f) != 74)
    {
        return false;
    }
    if (TruncDEXContribution(1, 0.5f) != 0 || TruncDEXContribution(100, 0.5f) != 50)
    {
        return false;
    }

    // --- PC vs non-PC food (critical parity difference) ---
    if (FoodPCBonus(100, 50, 20) != 20 || FoodPCBonus(100, 50, 1000) != 50)
    {
        return false;
    }
    if (FoodPCBonus(99, 1, 100) != 0) // 0.99f → int16 0
    {
        return false;
    }
    if (FinalizePCACC(100, 50, 20) != 120 || FinalizePCACC(-5, 0, 0) != 0)
    {
        return false;
    }

    // Non-PC: (100 + p*ACC)/100 — note +1 vs pure percent when cap allows
    if (FoodNonPCBonus(100, 50, 20) != 20)
    {
        return false;
    }
    if (FoodNonPCBonus(100, 10, 100) != 11) // PC would be 10
    {
        return false;
    }
    if (FoodPCBonus(100, 10, 100) != 10)
    {
        return false;
    }
    if (FoodNonPCBonus(100, 0, 0) != 0 || FoodNonPCBonus(100, 0, 100) != 1)
    {
        return false;
    }
    if (FinalizeNonPCACC(-5, 0, 0) != 1)
    {
        return false;
    }
    if (FinalizePCACC(200, 10, 100) != 220 || FinalizeNonPCACC(200, 10, 100) != 221)
    {
        return false;
    }

    // --- Full ResolveACC cases (parity with Go accstat_test) ---

    // PC one-hand: skill 200 + DEX 100*0.75 → 275
    {
        ACCParams p{};
        p.isPC          = true;
        p.skillLevel    = 200;
        p.dex           = 100;
        p.dexMultiplier = 0.75f;
        if (ResolveACC(p) != 275)
        {
            return false;
        }
    }

    // PC two-hand override + TWOHAND_ACC
    {
        ACCParams p{};
        p.isPC                   = true;
        p.skillLevel             = 200;
        p.dex                    = 100;
        p.dexMultiplier          = 0.5f; // overridden
        p.mainIsTwoHanded        = true;
        p.twoHandedDEXMultiplier = 0.75f;
        p.twoHandACC             = 15;
        if (ResolveACC(p) != 290)
        {
            return false;
        }
    }

    // PC skill curve + mods + merit
    {
        ACCParams p{};
        p.isPC           = true;
        p.skillLevel     = 500;
        p.dexMultiplier  = 0.75f;
        p.accMod         = 10;
        p.offsetAccuracy = 5;
        p.meritAccuracy  = 3;
        if (ResolveACC(p) != 478)
        {
            return false;
        }
    }

    // PC enlight + tandem + food
    {
        ACCParams p{};
        p.isPC              = true;
        p.skillLevel        = 200;
        p.dexMultiplier     = 0.75f;
        p.hasEnlight        = true;
        p.enspellDMG        = 20;
        p.tandemStrikePower = 10;
        p.foodACCP          = 10;
        p.foodACCCap        = 40;
        if (ResolveACC(p) != 253)
        {
            return false;
        }
    }

    // Automaton
    {
        ACCParams p{};
        p.isPet          = true;
        p.isAutomaton    = true;
        p.skillLevel     = 200;
        p.dex            = 100;
        p.accMod         = 10;
        p.offsetAccuracy = 5;
        if (ResolveACC(p) != 265)
        {
            return false;
        }
        p.accMod             = 0;
        p.offsetAccuracy     = 0;
        p.tandemStrikePower  = 12;
        if (ResolveACC(p) != 262)
        {
            return false;
        }
        p.tandemStrikePower = 0;
        p.foodACCP          = 10;
        p.foodACCCap        = 100;
        // base 250; food (100+10*250)/100 = 26 → 276
        if (ResolveACC(p) != 276)
        {
            return false;
        }
    }

    // Other pet correlation
    {
        ACCParams p{};
        p.isPet                       = true;
        p.accMod                      = 100;
        p.dex                         = 50;
        p.monsterCorrelationAdvantage = true;
        p.enhancesMonsterCorrelation  = 15;
        if (ResolveACC(p) != 140)
        {
            return false;
        }
        p.monsterCorrelationAdvantage = false;
        if (ResolveACC(p) != 125)
        {
            return false;
        }
    }

    // Mob / non-pet
    {
        ACCParams p{};
        p.accMod         = 80;
        p.offsetAccuracy = 5;
        p.dex            = 40;
        if (ResolveACC(p) != 105)
        {
            return false;
        }
        p.offsetAccuracy = 0;
        p.hasEnlight     = true;
        p.enspellDMG     = 7;
        if (ResolveACC(p) != 107)
        {
            return false;
        }
    }

    // Floors
    {
        ACCParams p{};
        p.accMod = -100;
        if (ResolveACC(p) != 1)
        {
            return false;
        }
        ACCParams q{};
        q.isPC          = true;
        q.accMod        = -50;
        q.dexMultiplier = 0.75f;
        if (ResolveACC(q) != 0)
        {
            return false;
        }
    }

    // Combined PC 1H: skill 350 → 335; DEX 80*0.75=60; mod 25; merit 4; food 63 → 487
    {
        ACCParams p{};
        p.isPC          = true;
        p.skillLevel    = 350;
        p.dex           = 80;
        p.dexMultiplier = 0.75f;
        p.accMod        = 25;
        p.meritAccuracy = 4;
        p.foodACCP      = 15;
        p.foodACCCap    = 80;
        if (ResolveACC(p) != 487)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runAccStat1640SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "acc_stat_1640 self-tests failed\n";
        return false;
    }
    return true;
}
