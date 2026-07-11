#include "test_mob_death_1412.h"

#include "map/mob_death_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mob death 1412 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runMobDeath1412SelfTests() -> bool
{
    bool ok = expect(mobdeathhelpers::ShouldKillSummonerPet(true, true, true), "live summoner pet") &&
              expect(!mobdeathhelpers::ShouldKillSummonerPet(false, true, true), "missing pet") &&
              expect(!mobdeathhelpers::ShouldKillSummonerPet(true, false, true), "dead pet") &&
              expect(!mobdeathhelpers::ShouldKillSummonerPet(true, true, false), "non-summoner");
    ok = expect(mobdeathhelpers::ShouldDetachPlayerMaster(true, true, true), "attached player master") && ok;
    ok = expect(!mobdeathhelpers::ShouldDetachPlayerMaster(true, false, true), "different master pet") && ok;

    std::vector<int> calls;
    mobdeathhelpers::Apply(
        true, true, true,
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); }, [&]() { calls.push_back(8); });
    ok = expect(calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8 }, "full transition order") && ok;

    calls.clear();
    mobdeathhelpers::Apply(
        false, false, false,
        [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); },
        [&]() { calls.push_back(7); }, [&]() { calls.push_back(8); });
    ok = expect(calls == std::vector<int>{ 2, 3, 5, 6, 7 }, "optional branches skipped") && ok;
    return ok;
}
