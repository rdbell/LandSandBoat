#include "test_char_event_activation_1458.h"

#include "map/char_event_activation_capacity.h"

#include <iostream>
#include <vector>

auto runCharEventActivation1458SelfTests() -> bool
{
    std::vector<int> calls{};
    chareventactivationhelpers::ActivateState(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); return true; },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); return true; },
        [&]() { calls.push_back(7); },
        [&]() { calls.push_back(8); return true; },
        [&]() { calls.push_back(9); },
        [&]() { calls.push_back(10); return true; },
        [&](const bool locked) { calls.push_back(locked ? 11 : 110); });
    bool ok = calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

    calls.clear();
    chareventactivationhelpers::ActivateState(
        [&]() { calls.push_back(12); },
        [&]() { calls.push_back(13); },
        [&]() { calls.push_back(14); },
        [&]() { calls.push_back(15); return false; },
        [&]() { calls.push_back(16); },
        [&]() { calls.push_back(17); return false; },
        [&]() { calls.push_back(18); },
        [&]() { calls.push_back(19); return false; },
        [&]() { calls.push_back(20); },
        [&]() { calls.push_back(21); return false; },
        [&](const bool locked) { calls.push_back(!locked ? 22 : 220); });
    ok = calls == std::vector<int>{ 12, 13, 14, 15, 17, 19, 21, 22 } && ok;

    if (!ok)
    {
        std::cerr << "char event activation 1458 self-test failed\n";
    }
    return ok;
}
