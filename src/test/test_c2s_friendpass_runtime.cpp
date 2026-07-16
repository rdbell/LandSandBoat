#include "test_c2s_friendpass_runtime.h"
#include "map/packets/c2s/0x01b_friendpass.h"
#include <iostream>

auto runC2SFriendPassRuntimeSelfTests() -> bool
{
    const auto ok = !friendpasshelpers::BuildResponsePlan(0).issuePassCode && friendpasshelpers::BuildResponsePlan(1).issuePassCode && !friendpasshelpers::BuildResponsePlan(2).issuePassCode && friendpasshelpers::BuildResponsePlan(3).issuePassCode;
    if (!ok)
        std::cerr << "c2s FRIENDPASS runtime self-test failed\n";
    return ok;
}
