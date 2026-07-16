/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_group_comlink_make_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0c3_group_comlink_make.h"

namespace
{

auto expectPlan(const groupcomlinkmakehelpers::Plan actual, const groupcomlinkmakehelpers::Plan expected, const char* label) -> bool
{
    if (actual.sourceSlot != expected.sourceSlot || actual.attemptLinkpearlSpawn != expected.attemptLinkpearlSpawn || actual.createLinkpearl != expected.createLinkpearl)
    {
        std::cerr << "c2s GROUP_COMLINK_MAKE runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runC2SGroupComlinkMakeRuntimeSelfTests() -> bool
{
    using namespace groupcomlinkmakehelpers;

    bool ok = true;
    ok      = expectPlan(PlanFor(1, true, true), { SourceSlot::Linkshell1, true, true }, "linkshell 1 creates pearl") && ok;
    ok      = expectPlan(PlanFor(2, true, true), { SourceSlot::Linkshell2, true, true }, "linkshell 2 creates pearl") && ok;
    ok      = expectPlan(PlanFor(1, false, true), { SourceSlot::Linkshell1, false, false }, "missing linkshell does nothing") && ok;
    ok      = expectPlan(PlanFor(2, true, false), { SourceSlot::Linkshell2, true, false }, "failed pearl spawn does nothing") && ok;
    ok      = expectPlan(PlanFor(0, true, true), { SourceSlot::None, false, false }, "unknown linkshell does nothing") && ok;
    return ok;
}
