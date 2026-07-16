/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_alter_ego_points_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0c1_alter_ego_points.h"

auto runC2SAlterEgoPointsRuntimeSelfTests() -> bool
{
    if (alteregopointspackethelpers::SelectAction() != alteregopointspackethelpers::Action::SendAlterEgoPoints)
    {
        std::cerr << "c2s ALTER_EGO_POINTS runtime self-test failed: action is not SendAlterEgoPoints\\n";
        return false;
    }

    return true;
}
