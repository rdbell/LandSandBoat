#include "test_synth_failure_7087.h"

#include "map/synth_failure.h"

#include <iostream>

auto runSynthFailure7087SelfTests() -> bool
{
    using synthfailurehelpers::ShouldBroadcastFailure;

    const bool ok = !ShouldBroadcastFailure(ZONE_MONORAIL_PRE_RELEASE) && !ShouldBroadcastFailure(ZONE_49) && ShouldBroadcastFailure(299) &&
                    !ShouldBroadcastFailure(300);
    if (!ok)
    {
        std::cerr << "synth failure 7087 self-test failed\n";
    }
    return ok;
}
