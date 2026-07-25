#include "test_forced_synth_critical_fail_6889.h"

#include "map/forced_synth_critical_fail.h"

#include <iostream>

auto runForcedSynthCriticalFail6889SelfTests() -> bool
{
    const bool ok = forcedsynthhelpers::MakePlan("onTrade", "Aldo") == forcedsynthhelpers::Plan{
                                                                         .sourceFunction = "onTrade",
                                                                         .characterName  = "Aldo",
                                                                         .criticalFail   = true,
                                                                     };
    if (!ok)
    {
        std::cerr << "forced synth critical fail 6889 self-test failed\n";
    }
    return ok;
}
