#include "test_chocobo_dig_1595.h"

#include "map/chocobo_dig_capacity.h"

#include <iostream>

namespace
{
using namespace chocobodighelpers;

auto Check() -> bool
{
    if (!IsDiggingZone(100) || IsDiggingZone(5) || IsDiggingZone(0)) // 5 Uleguerand commented out
    {
        return false;
    }
    if (ZoneCooldownSeconds(0) != 60 || DigCooldownSeconds(0) != 15)
    {
        return false;
    }
    if (ZoneCooldownSeconds(10) != 10 || DigCooldownSeconds(10) != 3)
    {
        return false;
    }
    if (CooldownsReady(100, 50, 90, 0) || !CooldownsReady(110, 50, 90, 0))
    {
        return false;
    }
    if (DigCooldownAllowed(false, 1000, 0, 0, 0) || !DigCooldownAllowed(true, 1000, 0, 0, 0))
    {
        return false;
    }
    if (DigSkillCap(0) != 100 || DigSkillCap(9) != 1000)
    {
        return false;
    }
    if (!DigSkillUpRollSucceeds(15) || DigSkillUpRollSucceeds(16))
    {
        return false;
    }
    if (DigSkillUpIncrement(99, 100, 5) != 1 || DigSkillUpIncrement(100, 100, 1) != 0)
    {
        return false;
    }
    if (!DigRankIncreases(99, 1, 0) || DigRankIncreases(98, 1, 0))
    {
        return false;
    }
    if (!FatigueBlocksDig(100, 100) || FatigueBlocksDig(0, 50) || !PositionTooClose(4.9f) || PositionTooClose(5.0f))
    {
        return false;
    }
    if (DigRareRateAdjust(200, true) != 100 || DigRareRateAdjust(50, true) != 100)
    {
        return false;
    }
    if (MoonAdjustedRoll(800, 1.5) != 1000 || MoonAdjustedRoll(100, 0.5) != 50)
    {
        return false;
    }
    if (MoonRollMultiplier(0) != 0.5 || MoonRollMultiplier(50) != 1.5 || MoonRollMultiplier(25) != 1.0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runChocoboDig1595SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "chocobo_dig_1595 self-tests failed\n";
        return false;
    }
    return true;
}
