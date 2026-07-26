#include "test_synth_update_dispatch_7091.h"

#include "map/ai/states/synth_update.h"

#include <iostream>

auto runSynthUpdateDispatch7091SelfTests() -> bool
{
    using synthupdate::UpdateAction;
    using synthupdate::updateAction;

    const bool ok = updateAction(true, true) == UpdateAction::CriticalFail &&
                    updateAction(true, false) == UpdateAction::CriticalFail &&
                    updateAction(false, true) == UpdateAction::Done &&
                    updateAction(false, false) == UpdateAction::Countdown;
    if (!ok)
    {
        std::cerr << "synth Update dispatch 7091 self-test failed\n";
    }
    return ok;
}
