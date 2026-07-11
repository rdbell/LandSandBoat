#include "test_pet_zoning_restore_1419.h"

#include "map/pet_zoning_restore_capacity.h"

#include <iostream>
#include <vector>

auto runPetZoningRestore1419SelfTests() -> bool
{
    bool ok = petzoningrestorehelpers::RestoredTP(1234.75f) == 1234 &&
              petzoningrestorehelpers::RestoredTP(0.0f) == 0;
    const auto apply = [](bool spawned, bool master, bool jug, std::vector<int>& calls)
    {
        petzoningrestorehelpers::Apply(
            spawned, master, jug,
            [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
            [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
            [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); });
    };
    std::vector<int> calls;
    apply(false, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1 };
    calls.clear();
    apply(true, false, true, calls);
    ok = ok && calls.empty();
    calls.clear();
    apply(true, true, true, calls);
    ok = ok && calls == std::vector<int>{ 2, 3, 4, 5, 6 };
    calls.clear();
    apply(true, true, false, calls);
    ok = ok && calls == std::vector<int>{ 2, 3, 4 };
    if (!ok)
    {
        std::cerr << "pet zoning restore 1419 self-test failed\n";
    }
    return ok;
}
