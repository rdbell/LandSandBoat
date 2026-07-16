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

#include "base.h"

enum class GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID : uint8_t
{
    Linkshell1 = 1,
    Linkshell2 = 2,
};

// GROUP_COMLINK_MAKE chooses one of the two equipped linkshell slots, then
// tries to spawn a linkpearl from that linkshell's data. Character inventory
// ownership remains in the packet handler.
namespace groupcomlinkmakehelpers
{

enum class SourceSlot : uint8
{
    None,
    Linkshell1,
    Linkshell2,
};

struct Plan
{
    SourceSlot sourceSlot{};
    bool       attemptLinkpearlSpawn{};
    bool       createLinkpearl{};
};

[[nodiscard]] constexpr auto SelectSourceSlot(const uint8 linkshellId) -> SourceSlot
{
    switch (static_cast<GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID>(linkshellId))
    {
        case GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell1:
            return SourceSlot::Linkshell1;
        case GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID::Linkshell2:
            return SourceSlot::Linkshell2;
    }

    return SourceSlot::None;
}

[[nodiscard]] constexpr auto PlanFor(const uint8 linkshellId, const bool hasEquippedLinkshell, const bool linkpearlSpawned) -> Plan
{
    const auto sourceSlot = SelectSourceSlot(linkshellId);
    if (sourceSlot == SourceSlot::None || !hasEquippedLinkshell)
    {
        return { sourceSlot, false, false };
    }

    return { sourceSlot, true, linkpearlSpawned };
}

} // namespace groupcomlinkmakehelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00C3
// This packet is sent by the client when requesting to create a link pearl for an equipped linkshell.
GP_CLI_PACKET(GP_CLI_COMMAND_GROUP_COMLINK_MAKE,
              uint8_t State;       // PS2: State
              uint8_t LinkshellId; // PS2: (New; did not exist.)
);
