#include "test_trust_cast_finished_1426.h"

#include "map/trust_cast_finished_capacity.h"

#include <iostream>
#include <vector>

auto runTrustCastFinished1426SelfTests() -> bool
{
    std::vector<int> calls;
    int recastID = 0;
    trustcastfinishedhelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); return 321; },
        [&](int id) { calls.push_back(3); recastID = id; });

    const bool ok = calls == std::vector<int>{ 1, 2, 3 } && recastID == 321;
    if (!ok)
    {
        std::cerr << "trust cast finished 1426 self-test failed\n";
    }
    return ok;
}
