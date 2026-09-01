#include "map/ai/states/death_state_policy.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runDeathStatePolicy9280SelfTests() -> bool
{
    using namespace deathstatepolicy;
    const bool ok = !canChangeState() && !canFollowPath() && !canInterrupt();
    if (!ok)
    {
        std::cerr << "death state policy 9280 self-test failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("death-state-policy-9280", runDeathStatePolicy9280SelfTests);
