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

#include <cstdint>

enum class GP_CLI_COMMAND_EVENTEND_MODE : uint16_t
{
    End           = 0,
    UpdatePending = 1,
};

// EVENTEND's entity-independent decisions. The packet host invokes the
// selected Lua callback, then uses shouldEndCurrentEvent() after OnEventFinish
// has had a chance to start another event.
namespace eventendhelpers
{

enum class Callback
{
    None,
    Update,
    Finish,
};

struct Transition
{
    uint16   eventId              = 0;
    uint32   result               = 0;
    Callback callback             = Callback::None;
    bool     lockCharacter        = false;
    bool     disableEventSkipping = false;

    // EVENTEND always performs these host effects after its mode handling.
    bool emitEventRecvPending = true;
    bool updateHP             = true;

    [[nodiscard]] bool shouldEndCurrentEvent(const int32 currentEventId) const
    {
        return callback == Callback::Finish && currentEventId == static_cast<int32>(eventId);
    }
};

inline auto MakeTransition(
    const GP_CLI_COMMAND_EVENTEND_MODE mode,
    const uint16                       eventId,
    const uint32                       endPara,
    const int32                        currentOption,
    const bool                         isCutsceneOption) -> Transition
{
    auto transition  = Transition{};
    transition.eventId = eventId;
    // The production packet starts from uint32 EndPara, then assigns the
    // signed event option into that unsigned value. Keep that conversion: Lua
    // callbacks receive uint32, and -1 is represented as UINT32_MAX.
    transition.result = currentOption == 0 ? endPara : static_cast<uint32>(currentOption);

    switch (mode)
    {
        case GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending:
            transition.callback = Callback::Update;
            if (transition.result != UINT32_MAX && isCutsceneOption)
            {
                transition.lockCharacter        = true;
                transition.disableEventSkipping = true;
            }
            break;
        case GP_CLI_COMMAND_EVENTEND_MODE::End:
            transition.callback = Callback::Finish;
            break;
    }

    return transition;
}

} // namespace eventendhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x005B
// This packet is sent by the client when ending an event or updating a pending event status.
GP_CLI_PACKET(GP_CLI_COMMAND_EVENTEND,
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint32_t EndPara;   // PS2: EndPara
              uint16_t ActIndex;  // PS2: ActIndex
              uint16_t Mode;      // PS2: Mode
              uint16_t EventNum;  // PS2: EventNum
              uint16_t EventPara; // PS2: EventPara
);
