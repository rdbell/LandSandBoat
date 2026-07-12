#include "test_spikes_status_1532.h"

#include "map/spikes_status_capacity.h"

#include <iostream>

namespace
{
using spikesstatushelpers::ResolveSpikesStatusEffect;
using spikesstatushelpers::SpikesStatusAction;
using spikesstatushelpers::SpikesStatusLvlDiff;

auto Check() -> bool
{
    if (SpikesStatusLvlDiff(false, 50, 1) != 0 || SpikesStatusLvlDiff(true, 20, 10) != 10)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(4, true, 10, 10, false, false, false, 0) != SpikesStatusAction::ApplyCurse)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(4, true, 10, 10, true, false, false, 0) != SpikesStatusAction::None)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(2, true, 10, 10, false, false, false, 19) != SpikesStatusAction::ApplyParalysis)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(2, true, 10, 10, false, false, false, 20) != SpikesStatusAction::None)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(5, true, 10, 10, false, false, false, 29) != SpikesStatusAction::ApplyStun)
    {
        return false;
    }
    if (ResolveSpikesStatusEffect(5, true, 10, 10, false, false, true, 0) != SpikesStatusAction::None)
    {
        return false;
    }
    return true;
}
} // namespace

auto runSpikesStatus1532SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "spikes_status_1532 self-tests failed\n";
        return false;
    }
    return true;
}
