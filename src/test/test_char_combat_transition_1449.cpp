#include "test_char_combat_transition_1449.h"

#include "map/char_combat_transition_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

auto runCharCombatTransition1449SelfTests() -> bool
{
    std::vector<int> calls{};
    charcombattransitionhelpers::ChangeTarget(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); });
    bool ok = calls == std::vector<int>{ 1, 2, 3 };

    std::uint32_t battlesFought = std::numeric_limits<std::uint32_t>::max();
    calls.clear();
    charcombattransitionhelpers::Engage(
        battlesFought,
        [&]()
        {
            calls.push_back(4);
            battlesFought = std::numeric_limits<std::uint32_t>::max();
        },
        [&]()
        {
            calls.push_back(battlesFought == std::numeric_limits<std::uint32_t>::max() ? 5 : 50);
        });
    ok = battlesFought == 0 && calls == std::vector<int>{ 4, 5 } && ok;

    bool hasError = false;
    calls.clear();
    charcombattransitionhelpers::Disengage(
        [&]() { calls.push_back(6); },
        [&]()
        {
            calls.push_back(7);
            hasError = true;
        },
        [&]()
        {
            calls.push_back(8);
            return hasError;
        },
        [&]() { calls.push_back(9); },
        [&](bool weaponDrawn) { calls.push_back(!weaponDrawn ? 10 : 100); });
    ok = calls == std::vector<int>{ 6, 7, 8, 9, 10 } && ok;

    calls.clear();
    charcombattransitionhelpers::Disengage(
        [&]() { calls.push_back(11); },
        [&]() { calls.push_back(12); },
        [&]()
        {
            calls.push_back(13);
            return false;
        },
        [&]() { calls.push_back(14); },
        [&](bool weaponDrawn) { calls.push_back(!weaponDrawn ? 15 : 150); });
    ok = calls == std::vector<int>{ 11, 12, 13, 15 } && ok;

    if (!ok)
    {
        std::cerr << "char combat transition 1449 self-test failed\n";
    }
    return ok;
}
