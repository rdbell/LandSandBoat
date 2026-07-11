#include "test_mob_death_reward_1413.h"

#include "map/mob_death_reward_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "mob death reward 1413 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runMobDeathReward1413SelfTests() -> bool
{
    bool ok = expect(mobdeathrewardhelpers::HasValidLastAttacker(true, true), "valid attacker") &&
              expect(!mobdeathrewardhelpers::HasValidLastAttacker(false, true), "missing attacker") &&
              expect(!mobdeathrewardhelpers::HasValidLastAttacker(true, false), "reused target id");

    const auto apply = [](bool dead, bool valid, std::vector<int>& calls)
    {
        mobdeathrewardhelpers::Apply(
            dead, valid,
            [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); },
            [&]() { calls.push_back(3); }, [&]() { calls.push_back(4); },
            [&]() { calls.push_back(5); }, [&]() { calls.push_back(6); });
    };

    std::vector<int> calls;
    apply(true, true, calls);
    ok = expect(calls == std::vector<int>{ 1, 3, 4, 5, 6 }, "valid-attacker reward order") && ok;
    calls.clear();
    apply(true, false, calls);
    ok = expect(calls == std::vector<int>{ 2, 3, 4, 5, 6 }, "fallback reward order") && ok;
    calls.clear();
    apply(false, true, calls);
    ok = expect(calls.empty(), "revived mob no-op") && ok;
    return ok;
}
