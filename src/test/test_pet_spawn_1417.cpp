#include "test_pet_spawn_1417.h"

#include "map/pet_spawn_capacity.h"

#include <iostream>
#include <vector>

auto runPetSpawn1417SelfTests() -> bool
{
    bool ok = petspawnhelpers::ShouldInitializeElemental(true, true, true) &&
              !petspawnhelpers::ShouldInitializeElemental(false, true, true) &&
              !petspawnhelpers::ShouldInitializeElemental(true, false, true) &&
              !petspawnhelpers::ShouldInitializeElemental(true, true, false);

    const auto apply = [](bool elemental, bool jug, std::vector<int>& calls)
    {
        petspawnhelpers::Apply(
            elemental, jug,
            [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
            [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
            [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); });
    };
    std::vector<int> calls;
    apply(true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3, 4, 5, 6 };
    calls.clear();
    apply(false, false, calls);
    ok = ok && calls == std::vector<int>{ 5, 6 };
    if (!ok)
    {
        std::cerr << "pet spawn 1417 self-test failed\n";
    }
    return ok;
}
