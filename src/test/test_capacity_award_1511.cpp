#include "test_capacity_award_1511.h"

#include "map/capacity_award_capacity.h"

#include <iostream>

namespace
{
using capacityawardhelpers::ApplyExpRate;
using capacityawardhelpers::CapacityAwardMessage;
using capacityawardhelpers::SelectCapacityAwardMessage;
using capacityawardhelpers::ShouldAdvanceChainNumber;
using capacityawardhelpers::ShouldAwardCapacityPoints;
using capacityawardhelpers::ShouldFireRoeCapacity;
using capacityawardhelpers::ShouldRejectDead;

auto Check() -> bool
{
    if (!ShouldRejectDead(true) || ShouldRejectDead(false))
    {
        return false;
    }
    if (ApplyExpRate(100, 1.5f) != 150 || ApplyExpRate(100, 0.0f) != 0)
    {
        return false;
    }
    if (!ShouldAwardCapacityPoints(1) || ShouldAwardCapacityPoints(0))
    {
        return false;
    }
    if (SelectCapacityAwardMessage(true, true) != CapacityAwardMessage::CapacityChain ||
        SelectCapacityAwardMessage(true, false) != CapacityAwardMessage::CapacityPointsGained ||
        SelectCapacityAwardMessage(false, true) != CapacityAwardMessage::CapacityPointsGained)
    {
        return false;
    }
    if (!ShouldAdvanceChainNumber(true) || ShouldAdvanceChainNumber(false))
    {
        return false;
    }
    if (!ShouldFireRoeCapacity(false) || ShouldFireRoeCapacity(true))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCapacityAward1511SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "capacity award 1511 self-test failed\n";
    }
    return ok;
}
