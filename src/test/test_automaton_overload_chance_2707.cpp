#include "test_automaton_overload_chance_2707.h"

#include "map/entities/automaton_overload_chance.h"

#include <iostream>

auto runAutomatonOverloadChance2707SelfTests() -> bool
{
    using automatonoverloadhelpers::OverloadChance;
    const bool ok =
        OverloadChance(0, 0) == 0 &&
        OverloadChance(24, 0) == 0 &&
        OverloadChance(25, 0) == 0 &&
        OverloadChance(30, 0) == 5 &&
        OverloadChance(255, 0) == 230 && // 230 stays in uint8 range
        OverloadChance(255, -300) == 255 &&
        OverloadChance(10, -20) == 5 &&
        OverloadChance(10, 20) == 0;
    if (!ok)
    {
        std::cerr << "automaton overload chance 2707 failed\n";
    }
    return ok;
}
