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

#include "0x05c_eventendxzy.h"

#include "ai/ai_container.h"
#include "enmity_container.h"
#include "entities/char_entity.h"
#include "lua/luautils.h"
#include "notoriety_container.h"
#include "packets/s2c/0x052_eventucoff.h"
#include "packets/s2c/0x05b_wpos.h"
#include "packets/s2c/0x065_wpos2.h"

// Go host pure half: packetsystem.ValidateEventEndXZY / ProcessEventEndXZY /
// NewEventEndXZYHandler (6470); plan mappacket.ClientEventEndXZYTransitionFor.
auto GP_CLI_COMMAND_EVENTENDXZY::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .mustEqual(this->Mode, 1, "Mode not 1")
        .isInEvent(this->EventPara);
}

void GP_CLI_COMMAND_EVENTENDXZY::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto result  = this->EndPara;
    const auto eventId = this->EventPara;

    // TODO: Currently the return value for onEventUpdate in Interaction Framework is not received.  Remove
    // the localVar check when this is resolved.

    const int32 updateResult = luautils::OnEventUpdate(PChar, eventId, result);
    auto* const PPet         = PChar->PPet;
    const auto  transition   = eventendxzyhelpers::MakeTransition(
        updateResult,
        PChar->GetLocalVar("noPosUpdate"),
        PChar->loc.p,
        this->x,
        this->y,
        this->z,
        this->dir,
        PPet != nullptr,
        PPet != nullptr && PPet->isDead(),
        PPet != nullptr ? PPet->id : 0);

    if (transition.resetNoPositionUpdate)
    {
        PChar->SetLocalVar("noPosUpdate", 0);
    }

    PChar->pushPacket<GP_SERV_COMMAND_WPOS2>(PChar, transition.newPosition, static_cast<POSMODE>(transition.wpos2Mode));
    if (transition.emitWPos)
    {
        PChar->pushPacket<GP_SERV_COMMAND_WPOS>(PChar, transition.newPosition, static_cast<POSMODE>(transition.wposMode));
    }

    if (transition.repositionPet && PPet)
    {
        PPet->loc.p = transition.newPosition;

        if (transition.disengagePet)
        {
            PPet->PAI->Disengage();
        }

        // clear all enmity towards a charmed mob when it is teleported
        // use two loops to avoid modifying the container while iterating over it
        std::list<CMobEntity*> mobsToPacify;

        // first collect the mobs with hate towards the formerly charmed mob
        for (auto* entityWithEnmity : *PPet->PNotorietyContainer)
        {
            if (auto* mobToPacify = dynamic_cast<CMobEntity*>(entityWithEnmity))
            {
                mobsToPacify.emplace_back(mobToPacify);
            }
        }
        // then remove the formerly charmed mob from those mobs enmity containers
        if (const auto petId = transition.clearEnmityForId; petId)
        {
            for (const auto* mobToPacify : mobsToPacify)
            {
                mobToPacify->PEnmityContainer->Clear(*petId);
            }
        }
    }

    if (transition.emitEventRecvPending)
    {
        PChar->pushPacket<GP_SERV_COMMAND_EVENTUCOFF>(PChar, GP_SERV_COMMAND_EVENTUCOFF_MODE::EventRecvPending);
    }
}
