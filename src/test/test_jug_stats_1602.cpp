#include "test_jug_stats_1602.h"

#include "map/jug_stats_capacity.h"

#include <iostream>

namespace
{
using namespace jugstatshelpers;

auto Check() -> bool
{
    if (JugGrowth(1) != 1.0f || JugGrowth(26) != 1.07f || JugGrowth(76) != 1.22f)
    {
        return false;
    }
    if (JugMaxHP(1, 1.0f) != 17 || JugMaxHP(50, 1.0f) != 1359 || JugMaxHP(75, 1.0f) != 3023)
    {
        return false;
    }
    if (JugMaxHP(0, 1.0f) != 0)
    {
        return false;
    }
    if (JugMaxMP(1, 1.0f) != 15)
    {
        return false;
    }
    if (!IsJugCasterJob(JobWHM) || IsJugCasterJob(1))
    {
        return false;
    }
    if (JugStatBlend(10, 10) != 18)
    {
        return false;
    }
    return true;
}
} // namespace

auto runJugStats1602SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "jug_stats_1602 self-tests failed\n";
        return false;
    }
    return true;
}
