#include "test_c2s_group_change2_runtime.h"
#include "map/packets/c2s/0x077_group_change2.h"
#include <iostream>

auto runC2SGroupChange2RuntimeSelfTests() -> bool
{
    using namespace groupchange2helpers;
    const RuntimeState all{ true, true, true, true };
    const auto         party     = MakeDispatchPlan(0, 0, all);
    const auto         ls1       = MakeDispatchPlan(1, 2, all);
    const auto         ls2       = MakeDispatchPlan(2, 3, all);
    const auto         alliance  = MakeDispatchPlan(5, 1, all);
    const auto         missing   = MakeDispatchPlan(1, 2, { true, true, false, true });
    const auto         malformed = MakeDispatchPlan(3, 0, all);
    const auto         mismatch  = MakeDispatchPlan(0, 2, all);
    const bool         ok        = party.action == Action::AssignPartyRole &&
                                   ls1.action == Action::SendLinkshellRankChange && ls1.linkshellSlot == 1 &&
                                   ls2.action == Action::SendLinkshellRankChange && ls2.linkshellSlot == 2 &&
                                   alliance.action == Action::AssignAllianceLeaderAndReload &&
                                   missing.action == Action::None && malformed.action == Action::None && mismatch.action == Action::None;
    if (!ok)
        std::cerr << "c2s GROUP_CHANGE2 runtime self-test failed\n";
    return ok;
}
