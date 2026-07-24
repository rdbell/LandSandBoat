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

#include "0x06f_group_leave.h"

#include "entities/char_entity.h"
#include "enums/party_kind.h"

// Go host pure half: packetsystem.ValidateGroupLeave / ProcessGroupLeave /
// NewGroupLeaveHandler (6479); plan mappacket.ClientGroupLeaveMutationPlanFor.
auto groupleavehelpers::MakeMutationPlan(const PartyKind kind, const bool hasAlliance, const bool partyHasOnlyOneMember, const bool allianceHasOnlyOneParty, const bool isPartyLeader) -> MutationPlan
{
    switch (kind)
    {
        case PartyKind::Party:
            if (hasAlliance && partyHasOnlyOneMember)
            {
                return { allianceHasOnlyOneParty ? AllianceAction::Dissolve : AllianceAction::RemoveParty, true };
            }
            return { AllianceAction::None, true };
        case PartyKind::Alliance:
            if (hasAlliance && isPartyLeader)
            {
                return { allianceHasOnlyOneParty ? AllianceAction::Dissolve : AllianceAction::RemoveParty, false };
            }
            return {};
    }
    return {};
}

auto GP_CLI_COMMAND_GROUP_LEAVE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<PartyKind>(this->Kind)
        .mustNotEqual(PChar->PParty, nullptr, "Character is not in a party");
}

void GP_CLI_COMMAND_GROUP_LEAVE::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto plan = groupleavehelpers::MakeMutationPlan(
        this->Kind, PChar->PParty->m_PAlliance != nullptr, PChar->PParty->HasOnlyOneMember(), PChar->PParty->m_PAlliance != nullptr && PChar->PParty->m_PAlliance->hasOnlyOneParty(), PChar->PParty->GetLeader() == PChar);

    if (plan.allianceAction == groupleavehelpers::AllianceAction::Dissolve)
    {
        PChar->PParty->m_PAlliance->dissolveAlliance();
    }
    else if (plan.allianceAction == groupleavehelpers::AllianceAction::RemoveParty)
    {
        PChar->PParty->m_PAlliance->removeParty(PChar->PParty);
    }
    if (plan.removeMember)
    {
        PChar->PParty->RemoveMember(PChar);
    }
}
