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

#include "common/mmo.h"

#include <optional>

// EVENTENDXZY's entity-independent decisions. The packet host invokes the
// Lua callback before creating this plan, then sends the selected packets and
// applies the pet work to the live entities.
namespace eventendxzyhelpers
{

enum class PositionMode : uint8_t
{
    Normal = 0x00,
    Event  = 0x01,
    Clear  = 0x02,
};

struct Transition
{
    position_t newPosition{};
    bool       updatePosition        = false;
    bool       resetNoPositionUpdate = true;

    bool         emitWPos2 = true;
    PositionMode wpos2Mode = PositionMode::Clear;
    bool         emitWPos  = false;
    PositionMode wposMode  = PositionMode::Normal;

    // These effects are selected only for a present, living pet. The host
    // moves and disengages that pet, then clears this ID from every recorded
    // mob enmity container without mutating one while iterating it.
    bool                    repositionPet = false;
    bool                    disengagePet  = false;
    std::optional<uint32_t> clearEnmityForId;

    bool emitEventRecvPending = true;
};

[[nodiscard]] inline auto MakeTransition(
    const int32      onEventUpdateResult,
    const uint32     noPositionUpdate,
    const position_t currentPosition,
    const float      x,
    const float      y,
    const float      z,
    const int8       direction,
    const bool       hasPet,
    const bool       petIsDead,
    const uint32     petId) -> Transition
{
    auto transition           = Transition{};
    transition.newPosition    = currentPosition;
    transition.updatePosition = noPositionUpdate == 0 && onEventUpdateResult == 1;

    if (transition.updatePosition)
    {
        transition.newPosition = { x, y, z, 0, static_cast<uint8>(direction) };
        transition.wpos2Mode   = PositionMode::Event;
        transition.emitWPos    = true;
    }

    if (hasPet && !petIsDead)
    {
        transition.repositionPet    = true;
        transition.disengagePet     = true;
        transition.clearEnmityForId = petId;
    }

    return transition;
}

} // namespace eventendxzyhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x005C
// This packet is sent by the client when updating an event that involves the clients position. (ie. Requesting to warp between telepoints.)
GP_CLI_PACKET(GP_CLI_COMMAND_EVENTENDXZY,
              float    x;         // PS2: x
              float    y;         // PS2: y
              float    z;         // PS2: z
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint32_t EndPara;   // PS2: EndPara
              uint16_t EventNum;  // PS2: EventNum
              uint16_t EventPara; // PS2: EventPara
              uint16_t ActIndex;  // PS2: ActIndex
              uint8_t  Mode;      // PS2: Mode
              int8_t   dir;       // PS2: dir
);
