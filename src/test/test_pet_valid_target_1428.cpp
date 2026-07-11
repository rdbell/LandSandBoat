#include "test_pet_valid_target_1428.h"

#include "map/pet_valid_target_capacity.h"

#include <iostream>
#include <vector>

auto runPetValidTarget1428SelfTests() -> bool
{
    const auto apply = [](bool playerTarget, bool sameAllegiance, bool base, std::vector<int>& calls)
    {
        return petvalidtargethelpers::Apply(
            playerTarget,
            sameAllegiance,
            [&]() { calls.push_back(1); return base; });
    };

    std::vector<int> calls;
    bool ok = !apply(true, true, true, calls) && calls.empty();
    calls.clear();
    ok = ok && apply(false, true, true, calls) && calls == std::vector<int>{ 1 };
    calls.clear();
    ok = ok && !apply(true, false, false, calls) && calls == std::vector<int>{ 1 };
    if (!ok)
    {
        std::cerr << "pet valid target 1428 self-test failed\n";
    }
    return ok;
}
