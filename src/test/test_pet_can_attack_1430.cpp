#include "test_pet_can_attack_1430.h"

#include "map/pet_can_attack_capacity.h"

#include <iostream>
#include <vector>

auto runPetCanAttack1430SelfTests() -> bool
{
    const auto apply = [](bool master, bool pcMaster, bool owns, bool base, std::vector<int>& calls)
    {
        return petcanattackhelpers::Apply(
            master,
            [&]() { calls.push_back(1); return pcMaster; },
            [&]() { calls.push_back(2); return owns; },
            [&]() { calls.push_back(3); },
            [&]() { calls.push_back(4); },
            [&]() { calls.push_back(5); return base; });
    };

    std::vector<int> calls;
    bool ok = apply(false, false, false, true, calls) && calls == std::vector<int>{ 5 };
    calls.clear();
    ok = ok && !apply(true, false, false, false, calls) && calls == std::vector<int>{ 1, 5 };
    calls.clear();
    ok = ok && apply(true, true, true, true, calls) && calls == std::vector<int>{ 1, 2, 5 };
    calls.clear();
    ok = ok && !apply(true, true, false, true, calls) && calls == std::vector<int>{ 1, 2, 3, 4 };
    if (!ok)
    {
        std::cerr << "pet can attack 1430 self-test failed\n";
    }
    return ok;
}
