#include "test_pet_construction_1433.h"

#include "map/pet_construction_capacity.h"

#include <iostream>
#include <vector>

auto runPetConstruction1433SelfTests() -> bool
{
    std::vector<int> calls;
    petconstructionhelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); });

    const bool ok = calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7 };
    if (!ok)
    {
        std::cerr << "pet construction 1433 self-test failed\n";
    }
    return ok;
}
