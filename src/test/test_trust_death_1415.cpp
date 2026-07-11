#include "test_trust_death_1415.h"

#include "map/trust_death_capacity.h"

#include <iostream>
#include <vector>

auto runTrustDeath1415SelfTests() -> bool
{
    std::vector<int> calls;
    trustdeathhelpers::Apply(
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); });
    if (calls != std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 })
    {
        std::cerr << "trust death 1415 self-test failed: transition order\n";
        return false;
    }
    return true;
}
