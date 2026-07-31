#include "test_fstr_1559.h"

#include "map/fstr_capacity.h"

#include <iostream>

namespace
{
using fstrhelpers::GetFSTR;
using fstrhelpers::MeleeStatFactor;
using fstrhelpers::MobMeleeStatDiff;
using fstrhelpers::MobRangedStatDiff;
using fstrhelpers::PlayerStatDiffRaw;
using fstrhelpers::RangedStatFactor;
using fstrhelpers::SlotAmmo;
using fstrhelpers::SlotMain;
using fstrhelpers::SlotRanged;
using fstrhelpers::SlotSub;
using fstrhelpers::StatFactorActor;

auto Check() -> bool
{
    // Mob/pet melee ladder floor before level clamp.
    if (MobMeleeStatDiff(36) != 8 || MobMeleeStatDiff(26) != 5 || MobMeleeStatDiff(17) != 3 ||
        MobMeleeStatDiff(4) != 0 || MobMeleeStatDiff(-8) != -2 || MobMeleeStatDiff(-13) != -3 ||
        MobMeleeStatDiff(-19) != -4 || MobMeleeStatDiff(-32) != -7 || MobMeleeStatDiff(-42) != -10 ||
        MobMeleeStatDiff(-54) != -12 || MobMeleeStatDiff(-67) != -15 || MobMeleeStatDiff(-76) != -17 ||
        MobMeleeStatDiff(-77) != -17)
    {
        return false;
    }

    // Mob/pet ranged ladder floor before level clamp.
    if (MobRangedStatDiff(36) != 16 || MobRangedStatDiff(26) != 11 || MobRangedStatDiff(15) != 6 ||
        MobRangedStatDiff(4) != 1 || MobRangedStatDiff(-8) != -4 || MobRangedStatDiff(-16) != -8 ||
        MobRangedStatDiff(-31) != -15 || MobRangedStatDiff(-42) != -20 || MobRangedStatDiff(-53) != -25 ||
        MobRangedStatDiff(-64) != -30 || MobRangedStatDiff(-76) != -35 || MobRangedStatDiff(-77) != -35)
    {
        return false;
    }

    // Shared player/trust stat-difference ladder boundaries.
    if (PlayerStatDiffRaw(12) != 16 || PlayerStatDiffRaw(11) != 17 || PlayerStatDiffRaw(6) != 12 ||
        PlayerStatDiffRaw(5) != 12 || PlayerStatDiffRaw(1) != 8 || PlayerStatDiffRaw(0) != 8 ||
        PlayerStatDiffRaw(-2) != 6 || PlayerStatDiffRaw(-3) != 6 || PlayerStatDiffRaw(-7) != 2 ||
        PlayerStatDiffRaw(-8) != 2 || PlayerStatDiffRaw(-15) != -5 || PlayerStatDiffRaw(-16) != -4 ||
        PlayerStatDiffRaw(-21) != -9 || PlayerStatDiffRaw(-22) != -9)
    {
        return false;
    }

    // PC baseline: STR=VIT=50, rank=3, lvl=75 → melee 2, ranged 4
    if (MeleeStatFactor(StatFactorActor::PC, 75, 50, 50, 3) != 2)
    {
        return false;
    }
    if (RangedStatFactor(StatFactorActor::PC, 75, 50, 50, 3) != 4)
    {
        return false;
    }

    // Slot dispatch
    if (GetFSTR(SlotMain, StatFactorActor::PC, 75, 50, 50, 3) != 2 ||
        GetFSTR(SlotSub, StatFactorActor::PC, 75, 50, 50, 3) != 2 ||
        GetFSTR(SlotRanged, StatFactorActor::PC, 75, 50, 50, 3) != 4 ||
        GetFSTR(SlotAmmo, StatFactorActor::PC, 75, 50, 50, 3) != 4 ||
        GetFSTR(4, StatFactorActor::PC, 75, 50, 50, 3) != 0)
    {
        return false;
    }

    // Mob early return
    if (MeleeStatFactor(StatFactorActor::Mob, 1, 100, 0, 0) != 1 ||
        MeleeStatFactor(StatFactorActor::Mob, 0, 100, 0, 0) != 1)
    {
        return false;
    }
    // Pet lvl 1 high STR → upper clamp 5
    if (MeleeStatFactor(StatFactorActor::Pet, 1, 100, 0, 0) != 5)
    {
        return false;
    }
    // PC ignores early return: dSTR0 rank0 → 2
    if (MeleeStatFactor(StatFactorActor::PC, 1, 50, 50, 0) != 2)
    {
        return false;
    }

    // Mob melee ladder clamp pins (lvl 75 → [14, 20])
    if (MeleeStatFactor(StatFactorActor::Mob, 75, 200, 0, 0) != 20 ||
        MeleeStatFactor(StatFactorActor::Mob, 75, 0, 200, 0) != 14 ||
        MeleeStatFactor(StatFactorActor::Mob, 75, 64, 0, 0) != 15 ||
        MeleeStatFactor(StatFactorActor::Mob, 75, 84, 0, 0) != 20)
    {
        return false;
    }

    // Ranged mob early
    if (RangedStatFactor(StatFactorActor::Mob, 1, 100, 0, 0) != 1)
    {
        return false;
    }
    // PC ranged dSTR0: raw 8 / 2 = 4 (rank0 or rank3 both allow 4)
    if (RangedStatFactor(StatFactorActor::PC, 75, 50, 50, 0) != 4 ||
        RangedStatFactor(StatFactorActor::PC, 75, 50, 50, 3) != 4)
    {
        return false;
    }

    // ClassifyActor
    if (fstrhelpers::ClassifyActor(true, false) != StatFactorActor::Mob ||
        fstrhelpers::ClassifyActor(false, true) != StatFactorActor::Pet ||
        fstrhelpers::ClassifyActor(false, false) != StatFactorActor::PC)
    {
        return false;
    }
    // isMob takes priority over isPet if both true (shouldn't happen)
    if (fstrhelpers::ClassifyActor(true, true) != StatFactorActor::Mob)
    {
        return false;
    }

    return true;
}
} // namespace

auto runFSTR1559SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "fstr_1559 self-tests failed\n";
        return false;
    }
    return true;
}
