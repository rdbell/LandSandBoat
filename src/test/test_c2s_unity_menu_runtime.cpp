/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_unity_menu_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x116_unity_menu.h"

auto runC2SUnityMenuRuntimeSelfTests() -> bool
{
    const auto plan = unitymenuhelpers::SelectProcessPlan();
    if (!plan.sendUnityResponse || !plan.sendUnityPackets)
    {
        std::cerr << "c2s UNITY_MENU runtime self-test failed: valid process must send Unity response then Unity packets\n";
        return false;
    }

    return true;
}
