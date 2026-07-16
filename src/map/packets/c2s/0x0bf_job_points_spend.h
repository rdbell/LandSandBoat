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
#include "enums/msg_basic.h"

// JobPointsSpendActionPlan records JOB_POINTS_SPEND's host-facing
// post-validation work. The job-point store owns the mutation and supplies
// the resulting level after RaiseJobPoint completes.
namespace jobpointsspendpackethelpers
{

struct ActionPlan
{
    uint16_t raiseJobPointIndex;
    bool     sendMiscDataJobPoints;
    uint16_t sendJobPointsIndex;
    uint16_t battleMessageIndex;
    uint8_t  battleMessageLevel;
    MsgBasic battleMessage;
};

[[nodiscard]] constexpr auto MakeActionPlan(const uint16_t index, const uint8_t newLevel) -> ActionPlan
{
    return {
        .raiseJobPointIndex    = index,
        .sendMiscDataJobPoints = true,
        .sendJobPointsIndex    = index,
        .battleMessageIndex    = index,
        .battleMessageLevel    = newLevel,
        .battleMessage         = MsgBasic::JobPointsIncrease,
    };
}

} // namespace jobpointsspendpackethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00BF
// This packet is sent by the client when spending job points.
GP_CLI_PACKET(GP_CLI_COMMAND_JOB_POINTS_SPEND,
              uint16_t Index;     // PS2: Index
              uint16_t padding00; // PS2: padding00
);
