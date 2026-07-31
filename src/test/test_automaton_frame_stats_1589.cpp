#include "test_automaton_frame_stats_1589.h"

#include "map/automaton_frame_stats_capacity.h"

#include <iostream>

namespace
{
using namespace automatonframestatshelpers;

auto Check() -> bool
{
    const auto h1 = StatsAtLevel(FrameHarlequin, 1);
    if (!h1 || h1->maxHP != 52 || h1->maxMP != 10 || h1->STR != 8 || h1->DEX != 7)
    {
        return false;
    }
    const auto h99 = StatsAtLevel(FrameHarlequin, 99);
    if (!h99 || h99->maxHP != 2205 || h99->maxMP != 344 || h99->STR != 98)
    {
        return false;
    }
    const auto s99 = StatsAtLevel(FrameStormwaker, 99);
    if (!s99 || s99->maxHP != 1508 || s99->maxMP != 1189 || s99->MND != 99)
    {
        return false;
    }
    const auto s100 = StatsAtLevel(FrameStormwaker, 100);
    if (!s100 || s100->maxHP != s99->maxHP)
    {
        return false;
    }
    if (StatsAtLevel(FrameHarlequin, 0).has_value() || StatsAtLevel(0, 50).has_value())
    {
        return false;
    }
    if (CapStatsLevel(0) != 0 || CapStatsLevel(50) != 50 || CapStatsLevel(99) != 99 || CapStatsLevel(100) != 99 ||
        CapStatsLevel(255) != 99)
    {
        return false;
    }
    // Mid-level pin: Sharpshot 50 should exist with positive HP
    const auto sh50 = StatsAtLevel(FrameSharpshot, 50);
    if (!sh50 || sh50->maxHP == 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runAutomatonFrameStats1589SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "automaton_frame_stats_1589 self-tests failed\n";
        return false;
    }
    return true;
}
