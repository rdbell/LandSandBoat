#include "test_defense_gates_1580.h"

#include "map/defense_gates_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using namespace defensegateshelpers;

auto AlmostEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-9;
}

auto Check() -> bool
{
    // Can gates.
    if (!CanParry(true, true, false, true) || CanParry(false, true, false, true) || CanParry(true, false, false, true) ||
        CanParry(true, true, true, true) || CanParry(true, true, false, false))
    {
        return false;
    }
    if (!CanGuard(true, true, false, true) || CanGuard(true, false, false, true))
    {
        return false;
    }
    if (!CanBlock(true, false, true) || CanBlock(false, false, true) || CanBlock(true, true, true) || CanBlock(true, false, false))
    {
        return false;
    }
    // Block has no engaged inject.
    if (!CanBlock(true, false, true))
    {
        return false;
    }

    // Shared rate clamps keep the parry/guard and shield-block curves inside
    // their native bounds before flat or multiplier bonuses are applied.
    if (!AlmostEqual(ClampRate(4.0), 5.0) || !AlmostEqual(ClampRate(25.0), 25.0) ||
        !AlmostEqual(ClampRate(26.0), 25.0) || !AlmostEqual(ClampBlockRate(4.0), 5.0) ||
        !AlmostEqual(ClampBlockRate(100.0), 100.0) || !AlmostEqual(ClampBlockRate(101.0), 100.0))
    {
        return false;
    }

    // Ability eligibility.
    if (!PCParryAbilityEligible(true, true, false) || PCParryAbilityEligible(true, true, true) ||
        PCParryAbilityEligible(false, true, false) || PCParryAbilityEligible(true, false, false))
    {
        return false;
    }
    if (!PCGuardAbilityEligible(true, false, false) || !PCGuardAbilityEligible(true, true, true) ||
        PCGuardAbilityEligible(true, true, false) || PCGuardAbilityEligible(false, false, false))
    {
        return false;
    }
    if (!PCBlockAbilityEligible(true, true) || PCBlockAbilityEligible(true, false) || PCBlockAbilityEligible(false, true))
    {
        return false;
    }
    if (!NonPCParryAbilityEligible(true) || NonPCParryAbilityEligible(false))
    {
        return false;
    }
    if (!NonPCGuardAbilityEligible(true, true, false) || NonPCGuardAbilityEligible(true, true, true) ||
        NonPCGuardAbilityEligible(false, true, false) || NonPCGuardAbilityEligible(true, false, false))
    {
        return false;
    }
    if (!NonPCBlockAbilityEligible(true, true) || NonPCBlockAbilityEligible(false, true) || NonPCBlockAbilityEligible(true, false))
    {
        return false;
    }

    // Parry rate: skillDelta 0 → floor(10 + (0-6)/4) = floor(8.5) = 8, clamp 5..25.
    if (!AlmostEqual(ParryRate(100, 100, 0, 0), 8.0))
    {
        return false;
    }
    // skillDelta 20 → floor(10 + 14 / (60/9)) = floor(12.1) = 12.
    if (!AlmostEqual(ParryRate(120, 100, 0, 0), 12.0))
    {
        return false;
    }
    // Issekigan + Inquartata after clamp.
    if (!AlmostEqual(ParryRate(100, 100, 25, 5), 38.0))
    {
        return false;
    }

    // Guard rate: skillDelta 0 → floor(10 + 0/4) = 10.
    if (!AlmostEqual(GuardRate(100, 100, 0), 10.0))
    {
        return false;
    }
    // skillDelta 10 → floor(10 + 10/(60/9)) = floor(11.5) = 11 + additive.
    if (!AlmostEqual(GuardRate(110, 100, 5), 16.0))
    {
        return false;
    }

    // Block rate PC buckler 55, equal skill → clamp(55, 5, 100) = 55.
    if (!AlmostEqual(BlockRatePC(true, 1, 100, 100, 0, false, false), 55.0))
    {
        return false;
    }
    if (!AlmostEqual(BlockRatePC(false, 1, 100, 100, 0, false, false), 0.0))
    {
        return false;
    }
    // Reprisal default 1.5× after skill scale.
    // skillMod = (100*1.15 - 100)*0.2325 = 15*0.2325 = 3.4875
    // (55 + 3.4875) * 1.5 = 87.73125
    if (!AlmostEqual(BlockRatePC(true, 1, 100, 100, 0, true, false), 87.73125))
    {
        return false;
    }
    // Automaton early path.
    if (!AlmostEqual(BlockRateAutomaton(true, 40, 200, 100), 40.0 + 100.0 * 0.215))
    {
        return false;
    }
    if (!AlmostEqual(BlockRateAutomaton(false, 40, 200, 100), 0.0))
    {
        return false;
    }

    // Rolls.
    if (!DefenseRollSucceeds(20, 2000) || DefenseRollSucceeds(20, 2001))
    {
        return false;
    }
    if (!IsParried(true, 20, 2000) || IsParried(true, 20, 2001) || IsParried(false, 100, 1))
    {
        return false;
    }
    if (!IsGuarded(true, 40, 4000) || IsGuarded(true, 40, 4001))
    {
        return false;
    }
    if (!IsBlocked(true, 100, 10000) || IsBlocked(true, 55, 5501) || IsBlocked(false, 100, 1))
    {
        return false;
    }

    // Side-effect gates.
    if (!ShouldTryDefensiveSkillUp(true, true, true) || !ShouldTryDefensiveSkillUp(true, false, false) ||
        ShouldTryDefensiveSkillUp(true, false, true) || ShouldTryDefensiveSkillUp(false, true, false))
    {
        return false;
    }
    if (!ShouldApplyParryHPRecovery(true, 50, false) || ShouldApplyParryHPRecovery(true, 50, true) ||
        ShouldApplyParryHPRecovery(true, 0, false) || ShouldApplyParryHPRecovery(false, 50, false))
    {
        return false;
    }
    if (!ShouldApplyTacticalTP(true, true, true) || ShouldApplyTacticalTP(true, false, true) ||
        ShouldApplyTacticalTP(false, true, true) || ShouldApplyTacticalTP(true, true, false))
    {
        return false;
    }

    if (BlockRateFromShieldSize(6) != 100.0 || BlockRateFromShieldSize(0) != 0.0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runDefenseGates1580SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "defense_gates_1580 self-tests failed\n";
        return false;
    }
    return true;
}
