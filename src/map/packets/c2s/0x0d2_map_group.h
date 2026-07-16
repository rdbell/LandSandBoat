/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "base.h"

// MAP_GROUP's host-independent member filter. ForAlliance supplies the input
// sequence, including any requester entry it chooses to provide; matching
// entries are deliberately retained in that order.
namespace mapgrouppackethelpers
{

struct Member
{
    bool   present;
    uint16 zone;
    uint32 moghouseId;
};

[[nodiscard]] constexpr auto IsEligible(const uint16 requesterZone, const uint32 requesterMoghouseId, const Member& member) -> bool
{
    return member.present && member.zone == requesterZone && member.moghouseId == requesterMoghouseId;
}

[[nodiscard]] inline auto SelectEligible(const uint16 requesterZone, const uint32 requesterMoghouseId, const std::span<const Member> members) -> std::vector<std::size_t>
{
    auto selected = std::vector<std::size_t>{};
    selected.reserve(members.size());

    for (std::size_t index = 0; index < members.size(); ++index)
    {
        if (IsEligible(requesterZone, requesterMoghouseId, members[index]))
        {
            selected.push_back(index);
        }
    }

    return selected;
}

} // namespace mapgrouppackethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00D2
// This packet is sent by the client when requesting party member map marker information.
GP_CLI_PACKET(GP_CLI_COMMAND_MAP_GROUP,
              uint32_t ZoneNo; // PS2: ZoneNo
);
