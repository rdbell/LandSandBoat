#include "test_trust_spawn_1418.h"

#include "map/trust_spawn_capacity.h"

#include <iostream>
#include <vector>

auto runTrustSpawn1418SelfTests() -> bool
{
    std::vector<int> calls;
    trustspawnhelpers::Apply(
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); });
    const bool ok = calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 };
    if (!ok)
    {
        std::cerr << "trust spawn 1418 self-test failed: transition order\n";
    }
    return ok;
}
