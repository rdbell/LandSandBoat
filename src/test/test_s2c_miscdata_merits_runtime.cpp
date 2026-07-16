/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_miscdata_merits_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x063_miscdata_merits.h"

namespace
{

auto expectPlan(const miscdatameritshelpers::Plan actual, const miscdatameritshelpers::Plan expected, const char* label) -> bool
{
    if (actual.limitPoints != expected.limitPoints || actual.meritPoints != expected.meritPoints || actual.bluBonus != expected.bluBonus || actual.canUseMeritMode != expected.canUseMeritMode || actual.xpCappedOrMeritMode != expected.xpCappedOrMeritMode || actual.meritModeEnabled != expected.meritModeEnabled || actual.maxMeritPoints != expected.maxMeritPoints)
    {
        std::cerr << "s2c MISCDATA MERITS runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CMiscDataMeritsRuntimeSelfTests() -> bool
{
    using namespace miscdatameritshelpers;

    bool ok = true;
    ok = expectPlan(PlanFor({ 100, 9, true, 74, false, false, 4, 5, 74, 75, 900, 1000, 10, 2 }), { 100, 9, 0, false, false, false, 12 }, "BLU below threshold has no bonus or eligibility") && ok;
    ok = expectPlan(PlanFor({ 101, 10, true, 75, true, true, 4, 5, 75, 75, 999, 1000, 10, 2 }), { 101, 10, 4, true, true, true, 12 }, "level 75 BLU adds assimilation and capped XP enables merit") && ok;
    ok = expectPlan(PlanFor({ 102, 11, true, 99, true, false, 4, 5, 99, 99, 998, 1000, 10, 2 }), { 102, 11, 9, true, false, false, 12 }, "level 99 BLU adds job-point bonus but uncapped XP does not enable mode") && ok;
    ok = expectPlan(PlanFor({ 103, 12, false, 99, false, true, 4, 5, 80, 99, 1, 2, 250, 10 }), { 103, 12, 0, false, true, false, 4 }, "merit mode sets the display bit without eligibility and max points narrow") && ok;
    return ok;
}
