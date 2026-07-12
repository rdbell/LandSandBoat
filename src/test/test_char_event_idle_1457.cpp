#include "test_char_event_idle_1457.h"

#include "map/char_event_idle_capacity.h"

#include <iostream>
#include <optional>
#include <vector>

auto runCharEventIdle1457SelfTests() -> bool
{
    bool ok = chareventidlehelpers::ShouldDeferStart(true) &&
              !chareventidlehelpers::ShouldDeferStart(false);

    std::vector<int> calls{};
    chareventidlehelpers::RestoreIdle(
        [&]() { calls.push_back(1); },
        [&]()
        {
            calls.push_back(2);
            return std::optional<int>{ 123 };
        },
        [&](const int power) { calls.push_back(power == 123 ? 3 : 30); },
        [&]()
        {
            calls.push_back(4);
            return true;
        },
        [&](bool) { calls.push_back(5); },
        [&]() { calls.push_back(6); });
    ok = calls == std::vector<int>{ 1, 2, 3, 6 } && ok;

    calls.clear();
    chareventidlehelpers::RestoreIdle(
        [&]() { calls.push_back(7); },
        [&]()
        {
            calls.push_back(8);
            return std::optional<int>{};
        },
        [&](int) { calls.push_back(9); },
        [&]()
        {
            calls.push_back(10);
            return true;
        },
        [&](const bool dead) { calls.push_back(dead ? 11 : 110); },
        [&]() { calls.push_back(12); });
    ok = calls == std::vector<int>{ 7, 8, 10, 11, 12 } && ok;

    calls.clear();
    chareventidlehelpers::RestoreIdle(
        [&]() { calls.push_back(13); },
        [&]()
        {
            calls.push_back(14);
            return std::optional<int>{};
        },
        [&](int) { calls.push_back(15); },
        [&]()
        {
            calls.push_back(16);
            return false;
        },
        [&](const bool dead) { calls.push_back(!dead ? 17 : 170); },
        [&]() { calls.push_back(18); });
    ok = calls == std::vector<int>{ 13, 14, 16, 17, 18 } && ok;

    if (!ok)
    {
        std::cerr << "char event idle 1457 self-test failed\n";
    }
    return ok;
}
