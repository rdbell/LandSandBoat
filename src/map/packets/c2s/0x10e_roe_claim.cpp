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

#include "0x10e_roe_claim.h"

#include "entities/char_entity.h"
#include "packets/s2c/0x113_currencies_1.h"
#include "roe.h"

// Go host pure half: packetsystem.ValidateROEClaim / ProcessROEClaim /
// NewROEClaimHandler (6514); plan mappacket.ClientROEClaimRuntimeResultFor.
auto GP_CLI_COMMAND_ROE_CLAIM::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(settings::get<bool>("main.ENABLE_ROE"), true, "RoE is disabled")
        .range("ObjectiveId", this->ObjectiveId, 0, 4096);
}

void GP_CLI_COMMAND_ROE_CLAIM::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto result = roeclaimhelpers::SelectResult(roeutils::onRecordClaim(PChar, this->ObjectiveId));
    if (result.sendCurrencies1)
    {
        PChar->pushPacket<GP_SERV_COMMAND_CURRENCIES_1>(PChar);
    }
}
