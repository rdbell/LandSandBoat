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

#include "0x109_bazaar_open.h"

#include "entities/char_entity.h"

auto bazaaropenhelpers::applyRuntimeState(RuntimeState state) -> RuntimeState
{
    state.isSettingBazaarPrices = false;
    state.updateMask |= UPDATE_HP;
    return state;
}

// Go host pure half: packetsystem.ValidateBazaarOpen / ProcessBazaarOpen /
// NewBazaarOpenHandler (6506); plan mappacket.ClientBazaarOpenRuntimePlanFor.
auto GP_CLI_COMMAND_BAZAAR_OPEN::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(PChar->isSettingBazaarPrices, true, "isSettingBazaarPrices not true");
}

void GP_CLI_COMMAND_BAZAAR_OPEN::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto state = bazaaropenhelpers::applyRuntimeState({
        .isSettingBazaarPrices = PChar->isSettingBazaarPrices,
        .updateMask            = PChar->updatemask,
    });
    PChar->isSettingBazaarPrices = state.isSettingBazaarPrices;
    PChar->updatemask            = state.updateMask;
}
