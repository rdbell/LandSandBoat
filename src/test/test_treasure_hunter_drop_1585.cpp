#include "test_treasure_hunter_drop_1585.h"

#include "map/treasure_hunter_drop_capacity.h"

#include <iostream>

namespace
{
using namespace treasurehunterhelpers;

auto Check() -> bool
{
    if (GetDropRate(0, 10000) != 10000 || GetDropRate(14, 0) != 0)
    {
        return false;
    }
    // TH0 VC (2400+) → 2400
    if (GetDropRate(0, 2400) != 2400 || GetDropRate(0, 5000) != 2400)
    {
        return false;
    }
    // TH0 rare 500 → 500; TH9 rare 500 → 1150
    if (GetDropRate(0, 500) != 500 || GetDropRate(9, 500) != 1150)
    {
        return false;
    }
    // TH14 UR (rate < 50) → 150
    if (GetDropRate(14, 10) != 150)
    {
        return false;
    }
    // Clamp negative tier/rate
    if (GetDropRate(-1, 2400) != 2400 || GetDropRate(99, 500) != 2000)
    {
        return false;
    }
    if (DropBracket(2400) != 1 || DropBracket(1500) != 2 || DropBracket(0) != 7)
    {
        return false;
    }
    return true;
}
} // namespace

auto runTreasureHunterDrop1585SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "treasure_hunter_drop_1585 self-tests failed\n";
        return false;
    }
    return true;
}
