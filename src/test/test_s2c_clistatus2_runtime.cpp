#include "test_s2c_clistatus2_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x062_clistatus2.h"

auto runS2CCLIStatus2RuntimeSelfTests() -> bool
{
    using namespace clistatus2helpers;
    auto facts = Facts{};
    for (std::size_t index = 0; index < facts.workingSkills.size(); ++index)
    {
        facts.workingSkills[index] = static_cast<uint16>(0x1000 + index);
    }
    const auto plan = PlanFor(facts);
    for (std::size_t index = 0; index < plan.skillBase.size(); ++index)
    {
        const bool hidden   = index == 22 || index == 23 || index == 24;
        const auto expected = hidden ? HiddenSkillValue : facts.workingSkills[index];
        if (plan.skillBase[index] != expected)
        {
            std::cerr << "s2c CLISTATUS2 runtime self-test failed at skill " << index << '\n';
            return false;
        }
    }
    return true;
}
