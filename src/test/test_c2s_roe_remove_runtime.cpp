/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_roe_remove_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10d_roe_remove.h"

auto runC2SROERemoveRuntimeSelfTests() -> bool
{
    const auto plan = roeremovehelpers::SelectProcessPlan();
    if (!plan.deleteEminenceRecord || !plan.sendUnityResponse)
    {
        std::cerr << "c2s ROE_REMOVE runtime self-test failed: valid process must delete record then send Unity response\n";
        return false;
    }

    return true;
}
