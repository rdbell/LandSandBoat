#include "test_exp_distribute_1502.h"

#include "map/exp_distribute_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using expdistributehelpers::ActiveChainTimerSeconds;
using expdistributehelpers::ApplyChainMultiplier;
using expdistributehelpers::ApplyMonsterBonus;
using expdistributehelpers::ApplyPartyGapPenalty;
using expdistributehelpers::CapExpByLevel;
using expdistributehelpers::ChainMultiplier;
using expdistributehelpers::ChainTimerSeconds;
using expdistributehelpers::IsInPartyRange;
using expdistributehelpers::IsSyncTargetBlocking;
using expdistributehelpers::MaxTrackedPartySize;
using expdistributehelpers::MaxTrackedPCLevel;
using expdistributehelpers::ShouldApplyChain;
using expdistributehelpers::ShouldAwardBaseExp;
using expdistributehelpers::ShouldProcessMember;

auto Near(const float a, const float b, const float eps = 1e-4f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    if (!IsInPartyRange(99.9f) || IsInPartyRange(100.0f))
    {
        return false;
    }
    if (!IsSyncTargetBlocking(100.0f, false) || !IsSyncTargetBlocking(50.0f, true) || IsSyncTargetBlocking(50.0f, false))
    {
        return false;
    }

    // Gap no-exp
    if (!Near(ApplyPartyGapPenalty(100.0f, 60, 40, 20, 1000, 2000), 0.0f))
    {
        return false;
    }
    // High maxlevel ratio path
    if (!Near(ApplyPartyGapPenalty(100.0f, 60, 30, 0, 1000, 2000), 50.0f))
    {
        return false;
    }
    // Next-exp ratio path (maxlevel <= 50 and within +7)
    if (!Near(ApplyPartyGapPenalty(100.0f, 40, 35, 0, 1000, 2000), 50.0f))
    {
        return false;
    }

    if (!Near(ApplyMonsterBonus(100.0f, 50), 150.0f) || !Near(ApplyMonsterBonus(100.0f, 0), 100.0f))
    {
        return false;
    }

    if (!Near(CapExpByLevel(900.0f, 40), 400.0f) || !Near(CapExpByLevel(900.0f, 55), 500.0f) ||
        !Near(CapExpByLevel(900.0f, 70), 600.0f) || !Near(CapExpByLevel(100.0f, 70), 100.0f))
    {
        return false;
    }

    if (!Near(ChainMultiplier(0), 1.0f) || !Near(ChainMultiplier(1), 1.2f) || !Near(ChainMultiplier(5), 1.5f) ||
        !Near(ChainMultiplier(6), 1.55f) || !Near(ApplyChainMultiplier(100.0f, 1), 120.0f))
    {
        return false;
    }

    if (ChainTimerSeconds(10) != 50 || ChainTimerSeconds(11) != 100 || ChainTimerSeconds(61) != 360)
    {
        return false;
    }

    // Active chain refresh table samples.
    if (ActiveChainTimerSeconds(10, 0) != 50 || ActiveChainTimerSeconds(10, 5) != 6 ||
        ActiveChainTimerSeconds(10, 9) != 2 || ActiveChainTimerSeconds(25, 1) != 120 ||
        ActiveChainTimerSeconds(70, 3) != 165)
    {
        return false;
    }

    if (MaxTrackedPartySize(3, 5) != 5 || MaxTrackedPartySize(6, 5) != 6)
    {
        return false;
    }
    if (MaxTrackedPCLevel(40, 50) != 50 || MaxTrackedPCLevel(55, 50) != 55)
    {
        return false;
    }

    if (!ShouldProcessMember(true, false) || ShouldProcessMember(false, false) || ShouldProcessMember(true, true) ||
        ShouldProcessMember(false, true))
    {
        return false;
    }
    if (!ShouldAwardBaseExp(true) || ShouldAwardBaseExp(false) || !ShouldApplyChain(true) || ShouldApplyChain(false))
    {
        return false;
    }

    return true;
}
} // namespace

auto runExpDistribute1502SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "exp distribute 1502 self-test failed\n";
    }
    return ok;
}
