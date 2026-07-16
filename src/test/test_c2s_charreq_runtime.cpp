#include "test_c2s_charreq_runtime.h"
#include "map/packets/c2s/0x016_charreq.h"
#include <iostream>

auto runC2SCharReqRuntimeSelfTests() -> bool
{
    using A       = charreqhelpers::Action;
    using F       = charreqhelpers::Facts;
    const auto ok = charreqhelpers::SelectAction(F{ true, false, false, false, false }) == A::RefreshSelfAndStatus &&
                    charreqhelpers::SelectAction(F{ false, false, false, false, false }) == A::None &&
                    charreqhelpers::SelectAction(F{ false, true, true, true, false }) == A::None &&
                    charreqhelpers::SelectAction(F{ false, true, true, false, false }) == A::RefreshCharacter &&
                    charreqhelpers::SelectAction(F{ false, true, false, false, false }) == A::RefreshMob &&
                    charreqhelpers::SelectAction(F{ false, true, false, false, true }) == A::RefreshMogHouseNPC;
    if (!ok)
        std::cerr << "c2s CHARREQ runtime self-test failed\n";
    return ok;
}
