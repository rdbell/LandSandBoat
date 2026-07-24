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

#include "0x05b_eventend.h"

#include "entities/base_entity.h"
#include "entities/char_entity.h"
#include "lua/luautils.h"
#include "packets/s2c/0x052_eventucoff.h"

// Go host pure half: packetsystem.ValidateEventEnd / ProcessEventEnd /
// NewEventEndHandler (6469); plan mappacket.ClientEventEndTransitionFor.
auto GP_CLI_COMMAND_EVENTEND::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .oneOf<GP_CLI_COMMAND_EVENTEND_MODE>(this->Mode)
        .isInEvent(this->EventPara);
}

void GP_CLI_COMMAND_EVENTEND::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto mode = static_cast<GP_CLI_COMMAND_EVENTEND_MODE>(this->Mode);
    const auto result = PChar->currentEvent->option == 0
                            ? this->EndPara
                            : static_cast<uint32>(PChar->currentEvent->option);
    const auto transition = eventendhelpers::MakeTransition(
        mode,
        this->EventPara,
        this->EndPara,
        PChar->currentEvent->option,
        mode == GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending && result != UINT32_MAX &&
            PChar->currentEvent->hasCutsceneOption(static_cast<int32>(result)));

    if (transition.lockCharacter)
    {
        // If an optional cutscene starts, its selected option locks the player.
        PChar->setLocked(true);
    }
    if (transition.disableEventSkipping)
    {
        PChar->currentEvent->canSkip = false;
    }

    switch (transition.callback)
    {
        case eventendhelpers::Callback::Update:
            luautils::OnEventUpdate(PChar, transition.eventId, transition.result);
            break;
        case eventendhelpers::Callback::Finish:
            luautils::OnEventFinish(PChar, transition.eventId, transition.result);
            // Reset only when OnEventFinish did not start another event.
            if (transition.shouldEndCurrentEvent(PChar->currentEvent->eventId))
            {
                PChar->endCurrentEvent();
            }
            break;
        case eventendhelpers::Callback::None:
            break;
    }

    PChar->pushPacket<GP_SERV_COMMAND_EVENTUCOFF>(PChar, GP_SERV_COMMAND_EVENTUCOFF_MODE::EventRecvPending);
    PChar->updatemask |= UPDATE_HP;
}
