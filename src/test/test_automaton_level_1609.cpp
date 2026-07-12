#include "test_automaton_level_1609.h"

#include "map/automaton_level_capacity.h"

#include <iostream>

namespace
{
using namespace automatonlevelhelpers;

auto Check() -> bool
{
    if (MainLevel(true, 75, 37, 2) != 77 || MainLevel(false, 75, 37, 2) != 37 || MainLevel(true, 1, 1, -5) != 0)
    {
        return false;
    }
    if (SubLevel(99) != 49 || SubLevel(1) != 0 || SubLevel(0) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAutomatonLevel1609SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_level_1609 self-tests failed\n";
        return false;
    }
    return true;
}
