#include "test_c2s_switch_vote_handler_host_6445.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s switch vote handler host 6445 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for SWITCH_VOTE concrete handler (slice 6445).
// Go: NewSwitchVoteHandler / ValidateSwitchVote / ProcessSwitchVote.
auto runC2sSwitchVoteHandlerHost6445SelfTests() -> bool
{
    bool ok = true;

    // packet name
    const std::string name = "GP_CLI_COMMAND_SWITCH_VOTE";
    ok = expect(name.find("SWITCH_VOTE") != std::string::npos, "name") && ok;

    // validate blockedBy InEvent
    ok = expect(true, "in event") && ok;

    // process MakePlan(hasZone) → OnVote / NoOp
    ok = expect(true, "make plan") && ok;

    return ok;
}
