#include "test_c2s_dig_runtime.h"
#include "map/packets/c2s/0x063_dig.h"
#include <iostream>

auto runC2SDigRuntimeSelfTests() -> bool
{
    const auto plan = dighelpers::MakeDispatchPlan();
    if (!plan.ignored)
        std::cerr << "c2s DIG runtime self-test failed\n";
    return plan.ignored;
}
