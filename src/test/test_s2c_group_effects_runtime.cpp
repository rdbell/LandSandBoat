#include "test_s2c_group_effects_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x076_group_effects.h"

auto runS2CGroupEffectsRuntimeSelfTests() -> bool
{
    using namespace groupeffectshelpers;
    auto facts = std::vector<MemberFacts>(6);
    for (std::size_t index = 0; index < facts.size(); ++index)
    {
        facts[index].uniqueNo   = static_cast<uint32>(index + 1);
        facts[index].actIndex   = static_cast<uint16>(index + 10);
        facts[index].statusBits = index + 100;
        facts[index].buffs[0]   = static_cast<uint8>(index + 20);
    }
    const auto plan = PlanFor(facts);
    for (std::size_t index = 0; index < MemberCount; ++index)
    {
        if (plan.members[index].uniqueNo != facts[index].uniqueNo || plan.members[index].actIndex != facts[index].actIndex || plan.members[index].statusBits != facts[index].statusBits || plan.members[index].buffs != facts[index].buffs)
        {
            std::cerr << "s2c GROUP_EFFECTS runtime self-test failed: member copy\n";
            return false;
        }
    }
    const auto one = PlanFor({ facts[0] });
    if (plan.members[4].uniqueNo == facts[5].uniqueNo || one.members[1].uniqueNo != 0 || one.members[1].statusBits != 0)
    {
        std::cerr << "s2c GROUP_EFFECTS runtime self-test failed: cap or zero trailing member\n";
        return false;
    }
    return true;
}
