#include "test_s2c_roe_activelog_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x111_roe_activelog.h"

auto runS2CROEActiveLogRuntimeSelfTests() -> bool
{
    using namespace roeactiveloghelpers;

    auto facts              = Facts{};
    facts.records[0]        = { .id = 1, .count = 10 };
    facts.records[29]       = { .id = 30, .count = 300 };
    facts.records[30]       = { .id = 31, .count = 310 };
    const auto plan         = PlanFor(facts);
    const bool normalSlots  = plan.records[0].id == 1 && plan.records[0].count == 10 && plan.records[29].id == 30 && plan.records[29].count == 300;
    const bool timedSlot    = plan.records[63].id == 31 && plan.records[63].count == 310;
    bool       middleIsZero = true;
    for (std::size_t index = 30; index < 63; ++index)
    {
        middleIsZero = middleIsZero && plan.records[index].id == 0 && plan.records[index].count == 0;
    }
    if (!normalSlots || !timedSlot || !middleIsZero)
    {
        std::cerr << "s2c ROE_ACTIVELOG runtime self-test failed\n";
        return false;
    }
    return true;
}
