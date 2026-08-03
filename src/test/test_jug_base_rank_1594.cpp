#include "test_jug_base_rank_1594.h"

#include "map/base_to_rank_capacity.h"
#include "map/jug_base_capacity.h"

#include <iostream>

namespace
{
using namespace jugbasehelpers;
using namespace basetorankhelpers;

auto Check() -> bool
{
    // GetBaseToRank pins
    if (GetBaseToRank(1, 1) != 5 || GetBaseToRank(1, 75) != 5 + (74 * 50) / 100)
    {
        return false;
    }
    if (GetBaseToRank(7, 50) != 2 + (49 * 20) / 100 || GetBaseToRank(0, 50) != 0 || GetBaseToRank(8, 50) != 0)
    {
        return false;
    }

    // uint16_t promotes to int before the subtraction and multiplication.
    constexpr std::uint16_t maxLevel = 0xFFFF;
    if (GetBaseToRank(1, 0) != 5 || GetBaseToRank(2, 0) != 4 || GetBaseToRank(3, 0) != 4 ||
        GetBaseToRank(4, 0) != 3 || GetBaseToRank(5, 0) != 3 || GetBaseToRank(6, 0) != 2 ||
        GetBaseToRank(7, 0) != 2)
    {
        return false;
    }
    if (GetBaseToRank(1, maxLevel) != 32772 || GetBaseToRank(2, maxLevel) != 29494 ||
        GetBaseToRank(3, maxLevel) != 26217 || GetBaseToRank(4, maxLevel) != 22939 ||
        GetBaseToRank(5, maxLevel) != 19663 || GetBaseToRank(6, maxLevel) != 16385 ||
        GetBaseToRank(7, maxLevel) != 13108)
    {
        return false;
    }

    // JugBase low band L1 rank1 = 6
    if (JugBase(1, 1) != 6 || JugBase(7, 1) != 3)
    {
        return false;
    }
    // L50 still low band: rank1 = 6 + 49*3 = 153
    if (JugBase(1, 50) != 153)
    {
        return false;
    }
    // L51 high band: rank1 = 153 + 1*5 = 158
    if (JugBase(1, 51) != 158)
    {
        return false;
    }
    // L75 rank1 = 153 + 25*5 = 278
    if (JugBase(1, 75) != 278)
    {
        return false;
    }
    // L75 rank7 = 96 + 25*4.3 = 96 + 107.5 → 203
    if (JugBase(7, 75) != 203)
    {
        return false;
    }
    if (JugBase(0, 50) != 0 || JugBase(8, 50) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runJugBaseRank1594SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "jug_base_rank_1594 self-tests failed\n";
        return false;
    }
    return true;
}
