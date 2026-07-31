#include "test_physical_hit_rate_1571.h"

#include "map/physical_hit_rate_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace physicalhitratehelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

auto Check() -> bool
{
    // Shared integer clamp used by level-correction inputs.
    if (ClampInt(-120, -100, 50) != -100 || ClampInt(-100, -100, 50) != -100 ||
        ClampInt(25, -100, 50) != 25 || ClampInt(50, -100, 50) != 50 || ClampInt(75, -100, 50) != 50)
    {
        return false;
    }
    if (ClampFloat(-1.25, -1.0, 1.0) != -1.0 || ClampFloat(-1.0, -1.0, 1.0) != -1.0 ||
        ClampFloat(0.375, -1.0, 1.0) != 0.375 || ClampFloat(1.0, -1.0, 1.0) != 1.0 ||
        ClampFloat(1.25, -1.0, 1.0) != 1.0)
    {
        return false;
    }

    // HitRateCap table.
    if (!AlmostEqual(HitRateCap(true, false, false, false, false), CapPet) ||
        !AlmostEqual(HitRateCap(false, true, true, false, false), CapPCH2H) ||
        !AlmostEqual(HitRateCap(false, true, false, true, false), CapPCOffhandOr2H) ||
        !AlmostEqual(HitRateCap(false, true, false, false, true), CapPCOffhandOr2H) ||
        !AlmostEqual(HitRateCap(false, true, false, false, false), CapPCMainhand1H) ||
        !AlmostEqual(HitRateCap(false, false, false, false, false), CapNonPC))
    {
        return false;
    }

    // FlashPenalty.
    if (FlashPenalty(0) != 0 || FlashPenalty(-1) != 0 || FlashPenalty(12000) != 360)
    {
        return false;
    }

    // Level correction.
    if (LevelCorrectedAccuracy(100, 60, 50, false, false, false) != 100)
    {
        return false;
    }
    // non-PC +10 levels → +40
    if (LevelCorrectedAccuracy(100, 60, 50, true, false, false) != 140)
    {
        return false;
    }
    // PC under-level: dlvl=-10 → +(-40)
    if (LevelCorrectedAccuracy(100, 50, 60, true, true, false) != 60)
    {
        return false;
    }
    // Avatar clamp negative dlvl → 0 before non-PC branch (no change)
    if (LevelCorrectedAccuracy(100, 50, 60, true, false, true) != 100)
    {
        return false;
    }

    // Base ACC=EVA → 0.75
    if (!AlmostEqual(AccuracyEvasionToHitRate(100, 100), 0.75))
    {
        return false;
    }

    // MeleeHitRate baseline: acc=eva, cap 0.95 → 0.75
    {
        MeleeHitRateParams p{};
        p.acc = 200;
        p.eva = 200;
        p.cap = CapNonPC;
        if (!AlmostEqual(MeleeHitRate(p), 0.75))
        {
            return false;
        }
        // Floor clamp
        p.acc = 0;
        p.eva = 500;
        if (!AlmostEqual(MeleeHitRate(p), MeleeHitRateFloor))
        {
            return false;
        }
        // Cap clamp pet 0.99
        p.acc = 500;
        p.eva = 0;
        p.cap = CapPet;
        if (!AlmostEqual(MeleeHitRate(p), CapPet))
        {
            return false;
        }
    }

    // Ranged distance short-circuit
    {
        RangedHitRateParams p{};
        p.acc      = 200;
        p.eva      = 200;
        p.distance = 26.0;
        if (!AlmostEqual(RangedHitRate(p), 0.0))
        {
            return false;
        }
        p.distance = 10.0;
        if (!AlmostEqual(RangedHitRate(p), 0.75))
        {
            return false;
        }
        // Ranged floor 0.05
        p.acc = 0;
        p.eva = 500;
        if (!AlmostEqual(RangedHitRate(p), RangedHitRateFloor))
        {
            return false;
        }
    }

    // HitRateModifiers: flourish WS, Innin behind, closed position, ambush, yonin, flash.
    {
        HitRateModParams m{};
        m.isWeaponskill              = true;
        m.hasBuildingFlourish        = true;
        m.buildingFlourishPower      = 1;
        m.buildingFlourishSubPower   = 5; // 40+10=50
        m.hasInnin                   = true;
        m.inninPower                 = 30;
        m.inninJP                    = 2;
        m.isBehind23                 = true; // +32
        m.attackerIsPC               = true;
        m.attackerIsFacing           = true;
        m.attackerClosedPositionMerit = 5;
        m.hasAmbushTrait             = true;
        m.ambushMerit                = 10;
        m.hasYonin                   = true;
        m.yoninPower                 = 20;
        m.yoninJP                    = 3; // +20+6=26
        m.isFacing64                 = true;
        m.targetIsPC                 = true;
        m.targetIsFacing             = true;
        m.targetClosedPositionMerit  = 4;
        m.flashPenalty               = 15;

        const auto r = HitRateModifiers(m);
        // acc: 50 + 32 + 5 + 10 - 15 = 82
        // eva: 26 + 4 = 30
        if (r.accBonus != 82 || r.evaBonus != 30)
        {
            return false;
        }

        // Ranged skips flourish/innin/closed position attacker.
        m.isRanged = true;
        const auto rr = HitRateModifiers(m);
        // acc: ambush 10 - flash 15 = -5
        // eva: 26+4=30
        if (rr.accBonus != -5 || rr.evaBonus != 30)
        {
            return false;
        }
    }

    // Accuracy distance penalty
    if (AccuracyDistancePenalty(false, 25, 5, 0, 0, 75) != 0)
    {
        return false;
    }
    if (AccuracyDistancePenalty(true, 5, 5, 0, 0, 75) != 0) // inside centroid
    {
        return false;
    }
    // distance 25, centroidEnd 5, pct=1, floor(1 * 37.5)=37
    if (AccuracyDistancePenalty(true, 25, 5, 0, 0, 75) != 37)
    {
        return false;
    }

    // Sweet spot resolve
    {
        if (!IsYoichiWeapon(21210) || IsYoichiWeapon(1))
        {
            return false;
        }
        const auto y = ResolveSweetSpot(true, 21210, SkillArchery, 0);
        if (!AlmostEqual(y.end, SweetYoichi.end))
        {
            return false;
        }
        const auto lb = ResolveSweetSpot(true, 1, SkillArchery, SubSkillLongbow);
        if (!AlmostEqual(lb.end, SweetLongbow.end))
        {
            return false;
        }
        const auto th = ResolveSweetSpot(false, 0, 0, 0);
        if (!AlmostEqual(th.end, SweetThrowing.end))
        {
            return false;
        }
    }

    // HitRateToPercent floor
    if (HitRateToPercent(0.75) != 75 || HitRateToPercent(0.999) != 99 || HitRateToPercent(0.2) != 20)
    {
        return false;
    }

    // Attack distance penalty: PC outer edge at dist 25, centroidEnd 5, cSkill 200 → 20% of 200 = 40
    if (AttackDistancePenalty(false, 25, 0, 5, 0, 0, 200) != 0)
    {
        return false;
    }
    if (AttackDistancePenalty(true, 5, 0, 5, 0, 0, 200) != 0) // sweet spot
    {
        return false;
    }
    // distance 25, centroidEnd 5 → full outer 20% → ceil(0.2*200)=40
    if (AttackDistancePenalty(true, 25, 0, 5, 0, 0, 200) != 40)
    {
        return false;
    }
    // too close: distance 0, centroidStart 3 → -25% → abs(ceil(-0.25*200))=50
    if (AttackDistancePenalty(true, 0, 3, 6, 0, 0, 200) != 50)
    {
        return false;
    }

    return true;
}
} // namespace

auto runPhysicalHitRate1571SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "physical_hit_rate_1571 self-tests failed\n";
        return false;
    }
    return true;
}
