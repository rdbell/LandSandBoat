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

#include <algorithm>
#include <cstddef>

#include "base.h"

// Keeps CHAT_NAME's process-time routing and variable payload bound testable.
// Validation of unknown04 and unknown05 remains packet-owned.
namespace chatnamehelpers
{
enum class Action : uint8
{
    RejectJailed,
    HandleCustomMenu,
    ForwardTell,
};

constexpr auto MaxMessageLength = std::size_t{ 128 };
constexpr auto MessageOffset    = std::size_t{ 0x15 };

constexpr auto BoundedMessageLength(const std::size_t reportedPacketSize) -> std::size_t
{
    if (reportedPacketSize <= MessageOffset)
    {
        return 0;
    }

    return std::min(reportedPacketSize - MessageOffset, MaxMessageLength);
}

constexpr auto SelectAction(const bool jailed, const bool isCustomMenuTarget, const bool hasCustomMenuContext) -> Action
{
    if (jailed)
    {
        return Action::RejectJailed;
    }

    if (isCustomMenuTarget && hasCustomMenuContext)
    {
        return Action::HandleCustomMenu;
    }

    return Action::ForwardTell;
}
} // namespace chatnamehelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00B6
// This packet is sent by the client when sending tells to another player.
GP_CLI_PACKET_VLA(GP_CLI_COMMAND_CHAT_NAME, Mes,
                  uint8_t unknown04; // PS2: Dammy
                  uint8_t unknown05; // PS2: (New; did not exist.)
                  uint8_t sName[15]; // PS2: sName
                  uint8_t Mes[128];  // PS2: Mes (Variable length)
);
