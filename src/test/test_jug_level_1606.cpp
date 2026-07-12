#include "test_jug_level_1606.h"

#include "map/jug_level_capacity.h"

#include <iostream>

namespace
{
using namespace juglevelhelpers;

auto Check() -> bool
{
    if (WeaponDelay != 240)
    {
        return false;
    }
    if (CapLevel(75, 119) != 119 || CapLevel(99, 50) != 99)
    {
        return false;
    }
    if (CapHighestLevel(90, 75) != 75 || CapHighestLevel(50, 75) != 50)
    {
        return false;
    }
    if (LevelRangeWidth(0) != 3 || LevelRangeWidth(1) != 2 || LevelRangeWidth(2) != 1 || LevelRangeWidth(5) != 1 || LevelRangeWidth(-3) != 3)
    {
        return false;
    }
    if (ApplyLevelRandom(75, 2) != 73 || ApplyLevelRandom(1, 3) != 254)
    {
        return false;
    }
    if (FinalJugLevel(70, 99) != 70 || FinalJugLevel(99, 50) != 50)
    {
        return false;
    }
    if (RaiseWithBeastAffinity(75, 5) != 80 || RaiseWithBeastAffinity(250, 20) != 255)
    {
        return false;
    }
    return true;
}
} // namespace

auto runJugLevel1606SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "jug_level_1606 self-tests failed\n";
        return false;
    }
    return true;
}
