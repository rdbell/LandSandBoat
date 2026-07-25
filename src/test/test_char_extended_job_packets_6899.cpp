#include "test_char_extended_job_packets_6899.h"

#include "map/char_extended_job_packets.h"

#include <iostream>

auto runCharExtendedJobPackets6899SelfTests() -> bool
{
    using extendedjobpackethelpers::Action;
    using extendedjobpackethelpers::Plan;

    bool ok = true;
    ok      = extendedjobpackethelpers::BuildPlan(true, JOB_PUP, JOB_BLU) == Plan{ .actions = { Action::Monstrosity }, .count = 1 } && ok;
    ok      = extendedjobpackethelpers::BuildPlan(false, JOB_PUP, JOB_BLU) == Plan{ .actions = { Action::PUPMain, Action::BLUSub }, .count = 2 } && ok;
    ok      = extendedjobpackethelpers::BuildPlan(false, JOB_BLU, JOB_PUP) == Plan{ .actions = { Action::BLUMain, Action::PUPSub }, .count = 2 } && ok;
    ok      = extendedjobpackethelpers::BuildPlan(false, JOB_WAR, JOB_MNK) == Plan{} && ok;
    if (!ok)
    {
        std::cerr << "extended job packets 6899 self-test failed\n";
    }
    return ok;
}
