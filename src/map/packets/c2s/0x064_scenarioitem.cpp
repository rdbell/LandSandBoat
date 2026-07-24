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

#include "0x064_scenarioitem.h"

#include "entities/char_entity.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateScenarioItem / ProcessScenarioItem /
// NewScenarioItemHandler (6477); plan mappacket.ClientScenarioItemTransitionFor.
auto GP_CLI_COMMAND_SCENARIOITEM::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(this->UniqueNo, PChar->id, "Character ID mismatch")
        .mustEqual(this->ActIndex, PChar->targid, "Character targid mismatch")
        .range("TableIndex", this->TableIndex, 0, PChar->keys.tables.size());
}

void GP_CLI_COMMAND_SCENARIOITEM::process(MapSession* PSession, CCharEntity* PChar) const
{
    scenarioitemhelpers::ForEachMarkedKeyItem(this->TableIndex, this->LookItemFlag, [PChar](const uint16_t keyItemId)
                                              {
                                                  charutils::markSeenKeyItem(PChar, static_cast<KeyItem>(keyItemId));
                                              });

    if (scenarioitemhelpers::ShouldSaveKeyItems())
    {
        charutils::SaveKeyItems(PChar);
    }
}
