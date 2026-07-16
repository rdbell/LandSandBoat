#include "test_c2s_switch_vote_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0a1_switch_vote.h"

auto runC2SSwitchVoteRuntimeSelfTests() -> bool
{
    using switchvotehelpers::Action;
    using switchvotehelpers::MakePlan;

    const auto ok = MakePlan(false) == Action::NoOp && MakePlan(true) == Action::OnVote;
    if (!ok)
    {
        std::cerr << "c2s SWITCH_VOTE runtime self-test failed\n";
    }
    return ok;
}
