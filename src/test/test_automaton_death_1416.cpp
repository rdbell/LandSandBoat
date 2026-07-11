#include "test_automaton_death_1416.h"

#include "map/automaton_death_capacity.h"

#include <iostream>
#include <vector>

auto runAutomatonDeath1416SelfTests() -> bool
{
    std::vector<int> calls;
    automatondeathhelpers::Apply(true, [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); });
    bool ok = calls == std::vector<int>{ 1, 2 };
    calls.clear();
    automatondeathhelpers::Apply(false, [&]() { calls.push_back(1); }, [&]() { calls.push_back(2); });
    ok = ok && calls == std::vector<int>{ 2 };
    if (!ok)
    {
        std::cerr << "automaton death 1416 self-test failed: transition branches\n";
    }
    return ok;
}
