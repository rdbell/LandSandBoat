#include "test_c2s_group_checkid_runtime.h"
#include "map/packets/c2s/0x078_group_checkid.h"
#include <iostream>

auto runC2SGroupCheckIDRuntimeSelfTests() -> bool
{
    const auto plan = groupcheckidhelpers::MakeDispatchPlan();
    if (!plan.sendGroupCheckID)
        std::cerr << "c2s GROUP_CHECKID runtime self-test failed\n";
    return plan.sendGroupCheckID;
}
