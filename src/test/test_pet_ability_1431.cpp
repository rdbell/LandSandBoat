#include "test_pet_ability_1431.h"

#include "map/pet_ability_capacity.h"

#include <iostream>
#include <vector>

auto runPetAbility1431SelfTests() -> bool
{
    const auto apply = [](bool target, bool valid, bool range, bool instant, bool paralyzed, std::vector<int>& calls)
    {
        petabilityhelpers::Apply(
            target,
            [&]() { calls.push_back(1); return valid; },
            [&]() { calls.push_back(2); return range; },
            [&]() { calls.push_back(3); return instant; },
            [&]() { calls.push_back(4); return paralyzed; },
            [&]() { calls.push_back(5); },
            [&]() { calls.push_back(6); },
            [&]() { calls.push_back(7); },
            [&]() { calls.push_back(8); });
    };

    std::vector<int> calls;
    apply(false, true, false, false, false, calls);
    bool ok = calls == std::vector<int>{ 6, 8 };
    calls.clear();
    apply(true, false, false, false, false, calls);
    ok = ok && calls == std::vector<int>{ 1, 6, 8 };
    calls.clear();
    apply(true, true, true, false, false, calls);
    ok = ok && calls == std::vector<int>{ 1, 2 };
    calls.clear();
    apply(true, true, false, false, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 5, 8 };
    calls.clear();
    apply(true, true, false, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 7 };
    calls.clear();
    apply(true, true, false, true, false, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 8 };
    if (!ok)
    {
        std::cerr << "pet ability 1431 self-test failed\n";
    }
    return ok;
}
