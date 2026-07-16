/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_unity_toggle_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x118_unity_toggle.h"

auto runC2SUnityToggleRuntimeSelfTests() -> bool
{
    using unitytogglehelpers::MakeRuntimePlan;
    using unitytogglehelpers::RuntimePlan;

    const auto expect = [](const RuntimePlan actual, const RuntimePlan expected, const char* description)
    {
        if (actual.removeOnlineMember == expected.removeOnlineMember &&
            actual.addOnlineMember == expected.addOnlineMember &&
            actual.sendLocalPlayerPackets == expected.sendLocalPlayerPackets)
        {
            return true;
        }

        std::cerr << "c2s UNITY_TOGGLE runtime self-test failed: " << description << '\n';
        return false;
    };

    bool ok = true;
    ok = expect(MakeRuntimePlan(true, GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Inactive), { true, false, true }, "inactive removes existing member then refreshes") && ok;
    ok = expect(MakeRuntimePlan(true, GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Active), { true, true, true }, "active replaces existing member then refreshes") && ok;
    ok = expect(MakeRuntimePlan(false, GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Active), { false, true, true }, "active adds without a prior member") && ok;
    ok = expect(MakeRuntimePlan(false, GP_CLI_COMMAND_UNITY_TOGGLE_MODE::Inactive), { false, false, true }, "inactive without a member refreshes") && ok;
    return ok;
}
