#include "test_barrage_1544.h"

#include "map/barrage_capacity.h"

#include <iostream>

namespace
{
using barragehelpers::BarrageAmmoClamp;
using barragehelpers::BarrageLevelForJob;
using barragehelpers::BarrageShotCount;
using barragehelpers::BarrageSkillOK;
using barragehelpers::JobRNG;
using barragehelpers::SkillArchery;
using barragehelpers::SkillMarksmanship;

auto Check() -> bool
{
    if (BarrageShotCount(29) != 0 || BarrageShotCount(30) != 3 || BarrageShotCount(50) != 4 ||
        BarrageShotCount(75) != 5 || BarrageShotCount(90) != 6 || BarrageShotCount(99) != 7)
    {
        return false;
    }
    if (BarrageLevelForJob(JobRNG, 99, 10) != 99 || BarrageLevelForJob(1, 99, 30) != 30)
    {
        return false;
    }
    if (!BarrageSkillOK(false, 0) || !BarrageSkillOK(true, SkillArchery) || !BarrageSkillOK(true, SkillMarksmanship) ||
        BarrageSkillOK(true, 1))
    {
        return false;
    }
    if (BarrageAmmoClamp(3, true, 4) != 3 || BarrageAmmoClamp(3, true, 2) != 1 || BarrageAmmoClamp(3, false, 0) != 3)
    {
        return false;
    }
    return true;
}
} // namespace

auto runBarrage1544SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "barrage_1544 self-tests failed\n";
        return false;
    }
    return true;
}
