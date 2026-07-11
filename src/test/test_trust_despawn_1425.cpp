#include "test_trust_despawn_1425.h"

#include "map/trust_despawn_capacity.h"

#include <iostream>
#include <vector>

auto runTrustDespawn1425SelfTests() -> bool
{
    const auto apply = [](bool hasHP, std::vector<int>& calls)
    {
        trustdespawnhelpers::Apply(
            [&]() { calls.push_back(1); return hasHP; },
            [&]() { calls.push_back(2); },
            [&]() { calls.push_back(3); },
            [&]() { calls.push_back(4); });
    };

    std::vector<int> calls;
    apply(true, calls);
    bool ok = calls == std::vector<int>{ 1, 2, 3, 4 };
    calls.clear();
    apply(false, calls);
    ok = ok && calls == std::vector<int>{ 1, 3, 4 };
    if (!ok)
    {
        std::cerr << "trust despawn 1425 self-test failed\n";
    }
    return ok;
}
