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

// JOB_POINTS_REQ only responds after the player has unlocked job points with
// JOB_BREAKER. Key-item lookup itself remains owned by the character runtime.
namespace jobpointsreqhelpers
{

enum class Action : uint8
{
    None,
    SendJobPoints,
};

[[nodiscard]] constexpr auto SelectAction(const bool hasJobBreaker) -> Action
{
    return hasJobBreaker ? Action::SendJobPoints : Action::None;
}

} // namespace jobpointsreqhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00C0
// This packet is sent by the client when requesting job points information.
GP_CLI_PACKET(GP_CLI_COMMAND_JOB_POINTS_REQ);
