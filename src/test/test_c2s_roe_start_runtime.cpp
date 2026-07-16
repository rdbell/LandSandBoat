/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_roe_start_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x10c_roe_start.h"

auto runC2SROEStartRuntimeSelfTests() -> bool
{
    if (roestarthelpers::SelectAction(true) != roestarthelpers::Action::SendUnityAndTakeRecord)
    {
        std::cerr << "c2s ROE_START runtime self-test failed: added record did not send UNITY and take record\n";
        return false;
    }

    if (roestarthelpers::SelectAction(false) != roestarthelpers::Action::SendROEUnable)
    {
        std::cerr << "c2s ROE_START runtime self-test failed: rejected record did not send ROEUnable\n";
        return false;
    }

    return true;
}
