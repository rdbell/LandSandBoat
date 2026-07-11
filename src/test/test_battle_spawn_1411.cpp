#include "test_battle_spawn_1411.h"

#include "map/battle_spawn_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle spawn 1411 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runBattleSpawn1411SelfTests() -> bool
{
    std::vector<int> calls;
    battlespawnhelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); });
    return expect(calls == std::vector<int>{ 1, 2, 3, 4, 5 }, "spawn callback order");
}
