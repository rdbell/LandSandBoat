#include "test_pet_death_1414.h"

#include "map/pet_death_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "pet death 1414 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runPetDeath1414SelfTests() -> bool
{
    bool ok = expect(petdeathhelpers::ShouldDespawnForZoning(true, true, true, true), "zoning despawn") &&
              expect(!petdeathhelpers::ShouldDespawnForZoning(false, true, true, true), "dead pet") &&
              expect(!petdeathhelpers::ShouldDespawnForZoning(true, false, true, true), "missing master") &&
              expect(!petdeathhelpers::ShouldDespawnForZoning(true, true, false, true), "non-player master") &&
              expect(!petdeathhelpers::ShouldDespawnForZoning(true, true, true, false), "no respawn flag");
    ok = expect(petdeathhelpers::ShouldDetachPlayerMaster(true, true, true), "attached player master") && ok;
    ok = expect(!petdeathhelpers::ShouldDetachPlayerMaster(true, false, true), "different active pet") && ok;

    const auto apply = [](bool zoning, bool detach, std::vector<int>& calls)
    {
        petdeathhelpers::Apply(
            zoning, detach,
            [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
            [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
            [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); });
    };
    std::vector<int> calls;
    apply(true, true, calls);
    ok = expect(calls == std::vector<int>{ 1, 2, 4, 5, 6 }, "zoning order") && ok;
    calls.clear();
    apply(false, false, calls);
    ok = expect(calls == std::vector<int>{ 1, 3, 4, 5 }, "death-state order") && ok;
    return ok;
}
