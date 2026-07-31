#include "test_capacity_distribute_1501.h"

#include "map/capacity_distribute_capacity.h"

#include <cmath>
#include <iostream>

namespace
{
using capacitydistributehelpers::ApplyChainModifier;
using capacitydistributehelpers::BaseCapacityPoints;
using capacitydistributehelpers::ChainModifier;
using capacitydistributehelpers::HasNonZeroChainNumber;
using capacitydistributehelpers::LevelDiff;
using capacitydistributehelpers::NextChainNumberAfterAward;
using capacitydistributehelpers::ShouldAwardMember;
using capacitydistributehelpers::ShouldComputeCapacity;
using capacitydistributehelpers::ShouldResetInactiveChain;
using capacitydistributehelpers::ShouldSendChainMessage;

auto Near(const float a, const float b, const float eps = 1e-3f) -> bool
{
    return std::fabs(a - b) <= eps;
}

auto Check() -> bool
{
    if (ShouldAwardMember(false, false, true, true, 99) ||
        ShouldAwardMember(true, true, true, true, 99) ||
        ShouldAwardMember(true, false, false, true, 99) ||
        ShouldAwardMember(true, false, true, false, 99) ||
        ShouldAwardMember(true, false, true, true, 98) ||
        !ShouldAwardMember(true, false, true, true, 99))
    {
        return false;
    }

    if (ShouldComputeCapacity(99) || !ShouldComputeCapacity(100))
    {
        return false;
    }

    if (LevelDiff(100) != 1 || LevelDiff(110) != 11)
    {
        return false;
    }

    // d=1: 0.0089+0.0533+3.7439+89.7 = 93.5061
    if (!Near(BaseCapacityPoints(1), 93.5061f))
    {
        return false;
    }
    // d=0: 89.7
    if (!Near(BaseCapacityPoints(0), 89.7f))
    {
        return false;
    }

    if (!Near(ChainModifier(0), 1.0f) || !Near(ChainModifier(10), 1.5f) || !Near(ChainModifier(20), 1.5f) ||
        !Near(ChainModifier(5), 1.25f))
    {
        return false;
    }

    if (!Near(ApplyChainModifier(100.0f, 5), 125.0f))
    {
        return false;
    }

    if (ShouldResetInactiveChain(true) || !ShouldResetInactiveChain(false))
    {
        return false;
    }

    if (NextChainNumberAfterAward(0) != 1 || NextChainNumberAfterAward(3) != 4 || NextChainNumberAfterAward(65535) != 0)
    {
        return false;
    }

    if (!ShouldSendChainMessage(1, true) || ShouldSendChainMessage(0, true) || ShouldSendChainMessage(1, false))
    {
        return false;
    }

    if (!HasNonZeroChainNumber(1) || HasNonZeroChainNumber(0))
    {
        return false;
    }

    return true;
}
} // namespace

auto runCapacityDistribute1501SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "capacity distribute 1501 self-test failed\n";
    }
    return ok;
}
