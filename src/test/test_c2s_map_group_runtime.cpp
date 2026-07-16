/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_map_group_runtime.h"

#include <array>
#include <iostream>

#include "map/packets/c2s/0x0d2_map_group.h"

auto runC2SMapGroupRuntimeSelfTests() -> bool
{
    using mapgrouppackethelpers::Member;
    using mapgrouppackethelpers::SelectEligible;

    const auto members = std::array{
        Member{ .present = true, .zone = 100, .moghouseId = 7 },  // Retain the requester if ForAlliance supplies it.
        Member{ .present = false, .zone = 100, .moghouseId = 7 }, // Null alliance member.
        Member{ .present = true, .zone = 101, .moghouseId = 7 },
        Member{ .present = true, .zone = 100, .moghouseId = 8 },
        Member{ .present = true, .zone = 100, .moghouseId = 7 },
    };

    const auto selected = SelectEligible(100, 7, members);
    if (selected != std::vector<std::size_t>{ 0, 4 })
    {
        std::cerr << "c2s MAP_GROUP runtime self-test failed: selection must retain matching members in alliance order\n";
        return false;
    }

    return true;
}
