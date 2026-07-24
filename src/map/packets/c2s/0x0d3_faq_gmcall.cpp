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

#include "0x0d3_faq_gmcall.h"

#include "entities/char_entity.h"
#include "gmcall_packet_handlers.h"

// Go host pure half: packetsystem.ValidateFAQGMCall / ProcessFAQGMCall / NewFAQGMCallHandler (6566); disposition gmcall.PlanFAQGMCallType.
auto GP_CLI_COMMAND_FAQ_GMCALL::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<GP_CLI_COMMAND_FAQ_GMCALL_TYPE>(this->type)
        .mustEqual(this->vers, 0, "vers not equal to 0")
        .range("eos", this->eos, 0, 1);
}

void GP_CLI_COMMAND_FAQ_GMCALL::process(MapSession* PSession, CCharEntity* PChar) const
{
    gmcall::handler::HandleFAQGMCall(
        *this,
        [&](const GP_CLI_COMMAND_FAQ_GMCALL& packet)
        {
            return PChar->gmCallContainer().addPacket(packet);
        },
        [&]()
        {
            PChar->gmCallContainer().processCall(PChar);
        },
        [&]()
        {
            PChar->m_charHistory.gmCalls++;
        });
}
