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

#include "0x042_trophy_absence.h"

#include "entities/char_entity.h"
#include "treasure_pool.h"

// Go host pure half: packetsystem.ValidateTrophyAbsence / ProcessTrophyAbsence /
// NewTrophyAbsenceHandler (6460); plan mappacket.ClientTrophyAbsencePlanFor.
auto GP_CLI_COMMAND_TROPHY_ABSENCE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustNotEqual(PChar->PTreasurePool, nullptr, "Character does not have a treasure pool")
        .range("TrophyItemIndex", this->TrophyItemIndex, 0, TREASUREPOOL_SIZE - 1);
}

void GP_CLI_COMMAND_TROPHY_ABSENCE::process(MapSession* PSession, CCharEntity* PChar) const
{
    if (trophyabsencehelpers::MakePlan(PChar->PTreasurePool->hasPassedItem(PChar, this->TrophyItemIndex)).action == trophyabsencehelpers::Action::PassItem)
    {
        PChar->PTreasurePool->passItem(PChar, this->TrophyItemIndex);
    }
}
