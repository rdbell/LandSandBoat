/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_group_comlink_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x0e0_group_comlink.h"

namespace
{

auto expectPlan(const groupcomlinkhelpers::Plan actual, const groupcomlinkhelpers::Plan expected, const char* label) -> bool
{
    if (actual.sourceSlot != expected.sourceSlot || actual.linkshellNumber != expected.linkshellNumber || actual.itemIndex != expected.itemIndex || actual.category != expected.category)
    {
        std::cerr << "s2c GROUP_COMLINK runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CGroupComlinkRuntimeSelfTests() -> bool
{
    using namespace groupcomlinkhelpers;

    bool ok = true;
    ok = expectPlan(PlanFor(1, { true, 0x22, 0x33 }), { SourceSlot::Linkshell1, 1, 0x22, 0x33 }, "linkshell 1 selects first location") && ok;
    ok = expectPlan(PlanFor(2, { true, 0x44, 0x55 }), { SourceSlot::Linkshell2, 2, 0x44, 0x55 }, "linkshell 2 selects second location") && ok;
    ok = expectPlan(PlanFor(1, {}), { SourceSlot::Linkshell1, 1, 0, 0 }, "missing selected location stays zeroed") && ok;
    ok = expectPlan(PlanFor(9, { true, 0x66, 0x77 }), { SourceSlot::Linkshell2, 9, 0x66, 0x77 }, "non-one selects second location") && ok;
    return ok;
}
