#include "test_char_post_tick_refresh_1445.h"

#include "map/char_post_tick_refresh_capacity.h"

#include <iostream>
#include <vector>

auto runCharPostTickRefresh1445SelfTests() -> bool
{
    bool             reloadRequested = false;
    bool             effectsChanged  = false;
    bool             hasParty        = false;
    std::vector<int> calls{};

    charposttickrefreshhelpers::Apply(
        effectsChanged,
        [&]()
        {
            calls.push_back(1);
            reloadRequested = true;
        },
        [&]()
        {
            calls.push_back(2);
            return reloadRequested;
        },
        [&]()
        {
            calls.push_back(3);
            reloadRequested = false;
            effectsChanged  = true;
        },
        [&]() { calls.push_back(4); },
        [&]() { calls.push_back(5); },
        [&]() { calls.push_back(6); },
        [&]()
        {
            calls.push_back(7);
            hasParty = true;
        },
        [&]()
        {
            calls.push_back(8);
            return hasParty;
        },
        [&]() { calls.push_back(effectsChanged ? 9 : 90); });

    bool ok = !reloadRequested && !effectsChanged &&
              calls == std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    calls.clear();
    charposttickrefreshhelpers::Apply(
        effectsChanged,
        [&]() { calls.push_back(10); },
        [&]()
        {
            calls.push_back(11);
            return false;
        },
        [&]() { calls.push_back(12); },
        [&]() { calls.push_back(13); },
        [&]() { calls.push_back(14); },
        [&]() { calls.push_back(15); },
        [&]() { calls.push_back(16); },
        [&]()
        {
            calls.push_back(17);
            return true;
        },
        [&]() { calls.push_back(18); });
    ok = !effectsChanged && calls == std::vector<int>{ 10, 11 } && ok;

    effectsChanged = true;
    hasParty       = false;
    calls.clear();
    charposttickrefreshhelpers::Apply(
        effectsChanged,
        [&]() { calls.push_back(20); },
        []() { return false; },
        [&]() { calls.push_back(21); },
        [&]() { calls.push_back(22); },
        [&]() { calls.push_back(23); },
        [&]() { calls.push_back(24); },
        [&]() { calls.push_back(25); },
        [&]()
        {
            calls.push_back(26);
            return hasParty;
        },
        [&]() { calls.push_back(27); });
    ok = !effectsChanged && calls == std::vector<int>{ 20, 22, 23, 24, 25, 26 } && ok;

    if (!ok)
    {
        std::cerr << "char post-tick refresh 1445 self-test failed\n";
    }
    return ok;
}
