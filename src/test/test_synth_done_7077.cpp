#include "test_synth_done_7077.h"

#include "map/synth_done.h"

#include <iostream>

auto runSynthDone7077SelfTests() -> bool
{
    const auto missing = synthdonehelpers::MakePlan(false, false);
    const auto failure = synthdonehelpers::MakePlan(true, true);
    const auto success = synthdonehelpers::MakePlan(true, false);
    const bool ok      = missing == synthdonehelpers::Plan{ .resetStatus = true } &&
                         failure == synthdonehelpers::Plan{
                             .completion        = synthdonehelpers::Completion::Failure,
                             .skillUp           = true,
                             .commitTransaction = true,
                             .removeTransaction = true,
                             .resetStatus       = true,
                         } &&
                         success.completion == synthdonehelpers::Completion::Success && success.skillUp && success.commitTransaction && success.removeTransaction && success.resetStatus;
    if (!ok)
    {
        std::cerr << "synth done 7077 self-test failed\n";
    }
    return ok;
}
