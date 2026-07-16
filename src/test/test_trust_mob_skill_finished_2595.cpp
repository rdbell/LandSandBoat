#include "test_trust_mob_skill_finished_2595.h"

#include "map/trust_mob_skill_finished_capacity.h"

#include <iostream>

auto runTrustMobSkillFinished2595SelfTests() -> bool
{
    auto calls = 0;
    trustmobskillfinishedhelpers::Apply([&]() { ++calls; });

    if (calls != 1)
    {
        std::cerr << "trust mob skill finished 2595 self-test failed\n";
        return false;
    }
    return true;
}
