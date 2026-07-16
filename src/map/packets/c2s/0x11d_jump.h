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

// Keeps JUMP's process-time jail gate and target propagation independently
// testable. Scalar validation remains owned by GP_CLI_COMMAND_JUMP::validate.
namespace jumphelpers
{
enum class Action : uint8
{
    RejectInPrison,
    BroadcastSelfRange,
};

struct DispatchPlan
{
    Action   action;
    uint16_t actIndex;
};

constexpr auto SelectDispatchPlan(const bool inPrison, const uint16_t actIndex) -> DispatchPlan
{
    return {
        .action   = inPrison ? Action::RejectInPrison : Action::BroadcastSelfRange,
        .actIndex = actIndex,
    };
}
} // namespace jumphelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x011D
// This packet is sent by the client when using the jump command.
GP_CLI_PACKET(GP_CLI_COMMAND_JUMP,
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint16_t ActIndex;  // PS2: ActIndex
              uint16_t padding00; // Padding; unused.
);
