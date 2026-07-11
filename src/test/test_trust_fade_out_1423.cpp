#include "test_trust_fade_out_1423.h"

#include "map/trust_fade_out_capacity.h"

#include <iostream>
#include <vector>

auto runTrustFadeOut1423SelfTests() -> bool
{
    std::vector<int> calls;
    trustfadeouthelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); });

    const bool ok = calls == std::vector<int>{ 1, 2 };
    if (!ok)
    {
        std::cerr << "trust fade out 1423 self-test failed\n";
    }
    return ok;
}
