#include "test_automaton_post_tick_1421.h"

#include "map/automaton_post_tick_capacity.h"

#include <iostream>
#include <vector>

auto runAutomatonPostTick1421SelfTests() -> bool
{
    const auto apply = [](bool mask, bool disappeared, bool playerMaster, std::vector<int>& calls)
    {
        automatonposttickhelpers::Apply(
            mask,
            [&]() { calls.push_back(1); },
            [&]() { calls.push_back(2); return disappeared; },
            [&]() { calls.push_back(3); return playerMaster; },
            [&]() { calls.push_back(4); });
    };
    std::vector<int> calls;
    apply(true, false, true, calls);
    bool ok = calls == std::vector<int>{ 1, 2, 3, 4 };
    calls.clear();
    apply(false, false, true, calls);
    ok = ok && calls == std::vector<int>{ 1 };
    calls.clear();
    apply(true, true, true, calls);
    ok = ok && calls == std::vector<int>{ 1, 2 };
    calls.clear();
    apply(true, false, false, calls);
    ok = ok && calls == std::vector<int>{ 1, 2, 3 };
    if (!ok)
    {
        std::cerr << "automaton post tick 1421 self-test failed\n";
    }
    return ok;
}
