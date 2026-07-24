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

#include "0x118_unity_toggle.h"

#include "entities/char_entity.h"
#include "unitychat.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateUnityToggle / ProcessUnityToggle /
// NewUnityToggleHandler (6517); plan mappacket.ClientUnityToggleRuntimePlanFor.
auto GP_CLI_COMMAND_UNITY_TOGGLE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .oneOf<GP_CLI_COMMAND_UNITY_TOGGLE_MODE>(this->Mode);
}

void GP_CLI_COMMAND_UNITY_TOGGLE::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto plan = unitytogglehelpers::MakeRuntimePlan(PChar->PUnityChat != nullptr, static_cast<GP_CLI_COMMAND_UNITY_TOGGLE_MODE>(this->Mode));

    if (plan.removeOnlineMember)
    {
        unitychat::DelOnlineMember(PChar, PChar->PUnityChat->getLeader());
    }

    if (plan.addOnlineMember)
    {
        unitychat::AddOnlineMember(PChar, PChar->profile.unity_leader);
    }

    if (plan.sendLocalPlayerPackets)
    {
        charutils::SendLocalPlayerPackets(PChar);
    }
}
