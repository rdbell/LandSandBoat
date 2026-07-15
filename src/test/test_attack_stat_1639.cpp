#include "test_attack_stat_1639.h"

#include "map/attack_stat_capacity.h"

#include <iostream>

namespace
{
using namespace attackstathelpers;

auto Check() -> bool
{
    // Slot / constant pins
    if (SlotMain != 0 || SlotSub != 1 || SlotRanged != 2 || SlotAmmo != 3 || BaseATT != 8)
    {
        return false;
    }

    // --- STR multiplier resolution ---
    // non-PC always 0.5
    if (ResolveSTRAttackMultiplier(false, true, true, false, SlotMain, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 0.5f)
    {
        return false;
    }
    // PC 2H / H2H
    if (ResolveSTRAttackMultiplier(true, true, true, false, SlotMain, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 1.0f)
    {
        return false;
    }
    if (ResolveSTRAttackMultiplier(true, true, false, true, SlotMain, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 1.0f)
    {
        return false;
    }
    // PC one-hand main 0.75 / offhand 0.5
    if (ResolveSTRAttackMultiplier(true, true, false, false, SlotMain, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 0.75f)
    {
        return false;
    }
    if (ResolveSTRAttackMultiplier(true, true, false, false, SlotSub, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 0.5f)
    {
        return false;
    }
    // Ranged
    if (ResolveSTRAttackMultiplier(true, true, false, false, SlotRanged, 1.0f, 1.0f, 0.75f, 0.5f, 1.0f) != 1.0f)
    {
        return false;
    }

    // --- Smite ---
    if (ApplySmiteToATTP(10, 256, false) != 10)
    {
        return false;
    }
    if (ApplySmiteToATTP(0, 256, true) != 100 || ApplySmiteToATTP(5, 128, true) != 55)
    {
        return false;
    }
    if (ApplySmiteToATTP(0, 1, true) != 0 || ApplySmiteToATTP(0, 255, true) != 99)
    {
        return false;
    }

    // --- STR float truncation ---
    if (TruncSTRContribution(100, 0.5f) != 50 || TruncSTRContribution(100, 0.75f) != 75)
    {
        return false;
    }
    if (TruncSTRContribution(99, 0.75f) != 74 || TruncSTRContribution(1, 0.5f) != 0)
    {
        return false;
    }

    // --- Food cap / finalize integer division ---
    if (FoodATTBonus(100, 50, 20) != 20 || FoodATTBonus(100, 50, 1000) != 50)
    {
        return false;
    }
    if (FoodATTBonus(50, 1, 100) != 0)
    {
        return false;
    }
    if (FinalizeATT(100, 0, 50, 20) != 120 || FinalizeATT(100, 33, 0, 0) != 133)
    {
        return false;
    }
    if (FinalizeATT(50, 1, 0, 0) != 50 || FinalizeATT(8, -200, 0, 0) != 1)
    {
        return false;
    }

    // --- Full ResolveATT cases (parity with Go attackstat_test) ---

    // non-PC STR 100 * 0.5 → 58
    {
        ATTParams p{};
        p.str           = 100;
        p.strMultiplier = 0.5f;
        if (ResolveATT(p) != 58)
        {
            return false;
        }
    }

    // PC one-hand main 0.75 → 83; sub 0.5 → 58; 2H 1.0 → 108
    {
        ATTParams p{};
        p.isPC          = true;
        p.hasWeapon     = true;
        p.str           = 100;
        p.strMultiplier = 0.75f;
        if (ResolveATT(p) != 83)
        {
            return false;
        }
        p.strMultiplier = 0.5f;
        if (ResolveATT(p) != 58)
        {
            return false;
        }
        p.strMultiplier   = 1.0f;
        p.isTwoHandOrH2H  = true;
        if (ResolveATT(p) != 108)
        {
            return false;
        }
    }

    // Endark adds ENSPELL_DMG
    {
        ATTParams p{};
        p.hasEndark     = true;
        p.enspellDMG    = 20;
        p.strMultiplier = 0.5f;
        if (ResolveATT(p) != 28)
        {
            return false;
        }
    }

    // PC skill + ilvl skill
    {
        ATTParams p{};
        p.isPC            = true;
        p.hasWeapon       = true;
        p.weaponSkill     = 200;
        p.weaponILvlSkill = 50;
        p.strMultiplier   = 0.75f;
        if (ResolveATT(p) != 258)
        {
            return false;
        }
    }

    // Smite on 2H: ATTP+=100, ATT=8 → 16; ignored on 1H
    {
        ATTParams p{};
        p.isPC           = true;
        p.hasWeapon      = true;
        p.isTwoHandOrH2H = true;
        p.smiteMod       = 256;
        p.strMultiplier  = 1.0f;
        if (ResolveATT(p) != 16)
        {
            return false;
        }
        p.isTwoHandOrH2H = false;
        p.strMultiplier  = 0.75f;
        if (ResolveATT(p) != 8)
        {
            return false;
        }
    }

    // Food ATTP cap: ATT=100, 50% cap 20 → 120
    {
        ATTParams p{};
        p.attMod        = 92;
        p.strMultiplier = 0.5f;
        p.foodATTP      = 50;
        p.foodATTCap    = 20;
        if (ResolveATT(p) != 120)
        {
            return false;
        }
    }

    // Automaton melee skill
    {
        ATTParams p{};
        p.isPet               = true;
        p.isAutomaton         = true;
        p.str                 = 50;
        p.strMultiplier       = 0.5f;
        p.automatonMeleeSkill = 150;
        if (ResolveATT(p) != 183)
        {
            return false;
        }
    }

    // Pet monster correlation inject
    {
        ATTParams p{};
        p.isPet                       = true;
        p.strMultiplier               = 0.5f;
        p.monsterCorrelationAdvantage = true;
        p.enhancesMonsterCorrelation  = 25;
        if (ResolveATT(p) != 10)
        {
            return false;
        }
        p.monsterCorrelationAdvantage = false;
        if (ResolveATT(p) != 8)
        {
            return false;
        }
    }

    // Integer division / STR trunc parity
    {
        ATTParams p{};
        p.isPC          = true;
        p.hasWeapon     = true;
        p.str           = 99;
        p.strMultiplier = 0.75f;
        if (ResolveATT(p) != 82)
        {
            return false;
        }
        ATTParams q{};
        q.attMod        = 42;
        q.attP          = 1;
        q.strMultiplier = 0.5f;
        if (ResolveATT(q) != 50)
        {
            return false;
        }
    }

    // Floor at 1
    {
        ATTParams p{};
        p.attP          = -200;
        p.strMultiplier = 0.5f;
        if (ResolveATT(p) != 1)
        {
            return false;
        }
    }

    // Combined PC 2H: 852
    {
        ATTParams p{};
        p.isPC            = true;
        p.hasWeapon       = true;
        p.isTwoHandOrH2H  = true;
        p.str             = 100;
        p.strMultiplier   = 1.0f;
        p.weaponSkill     = 250;
        p.weaponILvlSkill = 30;
        p.smiteMod        = 256;
        p.attP            = 10;
        p.foodATTP        = 10;
        p.foodATTCap      = 40;
        if (ResolveATT(p) != 852)
        {
            return false;
        }
    }

    return true;
}
} // namespace

auto runAttackStat1639SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "attack_stat_1639 self-tests failed\n";
        return false;
    }
    return true;
}
