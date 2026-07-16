/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_mastery_display_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x11b_mastery_display.h"

auto runC2SMasteryDisplayRuntimeSelfTests() -> bool
{
    using masterydisplayhelpers::MakeRuntimePlan;
    using masterydisplayhelpers::RuntimePlan;

    const auto expect = [](const RuntimePlan actual, const RuntimePlan expected, const char* description)
    {
        if (actual.updateJobMasterDisplay == expected.updateJobMasterDisplay &&
            actual.jobMasterDisplay == expected.jobMasterDisplay &&
            actual.saveJobMasterDisplay == expected.saveJobMasterDisplay &&
            actual.sendCharStatus == expected.sendCharStatus &&
            actual.sendCharSync == expected.sendCharSync)
        {
            return true;
        }

        std::cerr << "c2s MASTERY_DISPLAY runtime self-test failed: " << description << '\n';
        return false;
    };

    bool ok = true;
    ok = expect(MakeRuntimePlan(false, GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::Off), { false, false, false, false, false }, "off does nothing when already off") && ok;
    ok = expect(MakeRuntimePlan(true, GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::On), { false, true, false, false, false }, "on does nothing when already on") && ok;
    ok = expect(MakeRuntimePlan(false, GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::On), { true, true, true, true, true }, "on persists and refreshes status and sync") && ok;
    ok = expect(MakeRuntimePlan(true, GP_CLI_COMMAND_MASTERY_DISPLAY_MODE::Off), { true, false, true, true, true }, "off persists and refreshes status and sync") && ok;
    return ok;
}
