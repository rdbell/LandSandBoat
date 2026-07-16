/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_myroom_is_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0cb_myroom_is.h"

namespace
{

auto expectPlan(const myroomishelpers::RemodelPlan actual, const myroomishelpers::RemodelPlan expected, const char* label) -> bool
{
    if (actual.oldStyle != expected.oldStyle || actual.appliedStyle != expected.appliedStyle || actual.mhflag != expected.mhflag || actual.warnSecondFloorLocked != expected.warnSecondFloorLocked || actual.warnMogPatioLocked != expected.warnMogPatioLocked || actual.saveCharStats != expected.saveCharStats || actual.sendSuccessfulRemodel != expected.sendSuccessfulRemodel || actual.requestZoneTransition != expected.requestZoneTransition)
    {
        std::cerr << "c2s MYROOM_IS runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runC2SMyRoomIsRuntimeSelfTests() -> bool
{
    using namespace myroomishelpers;

    bool ok = true;
    ok = expectPlan(PlanRemodel(618, { NATION_BASTOK, 0x0060, false }), { 615, 616, 0x00E0, false, true, true, true, true }, "locked patio uses Bastok default and re-zones from original request") && ok;
    ok = expectPlan(PlanRemodel(618, { NATION_WINDURST, 0x0180, true }), { 618, 618, 0x0180, true, false, true, true, false }, "unlocked patio preserves both style bits") && ok;
    ok = expectPlan(PlanRemodel(617, { NATION_SANDORIA, 0x0140, true }), { 617, 617, 0x0140, true, false, true, true, false }, "unchanged requested style does not re-zone") && ok;
    ok = expectPlan(PlanRemodel(616, { NATION_SANDORIA, 0x0000, true }), { 615, 616, 0x0080, true, false, true, true, false }, "style bits replace existing decoration bits") && ok;
    return ok;
}
