/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_map_markers_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x114_map_markers.h"

auto runC2SMapMarkersRuntimeSelfTests() -> bool
{
    if (mapmarkershelpers::SelectAction() != mapmarkershelpers::Action::SendHomepoints)
    {
        std::cerr << "c2s MAP_MARKERS runtime self-test failed: accepted requests must send HOMEPOINTS\n";
        return false;
    }

    return true;
}
