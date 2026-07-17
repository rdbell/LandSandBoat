/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "common/cbasetypes.h"

#include "packets/c2s/0x0a0_switch_proposal.h"

// Pure NominateProposal::inScope membership policy.
// Entity pointers stay host-owned; callers inject membership scalars.

namespace nominatehelpers
{

// InScopeForParty mirrors Party kind: matching party id, else non-zero
// proposal alliance id with matching member alliance.
inline auto InScopeForParty(
    const uint32 partyId,
    const uint32 allianceId,
    const bool   hasParty,
    const uint32 memberPartyId,
    const bool   hasAlliance,
    const uint32 memberAllianceId) -> bool
{
    if (!hasParty)
    {
        return false;
    }

    if (memberPartyId == partyId)
    {
        return true;
    }

    return allianceId != 0 && hasAlliance && memberAllianceId == allianceId;
}

// InScopeForLinkshell mirrors Linkshell1/Linkshell2 kinds: either equipped
// linkshell slot matches the proposal linkshell id.
inline auto InScopeForLinkshell(
    const uint32 linkshellId,
    const bool   hasLS1,
    const uint32 ls1Id,
    const bool   hasLS2,
    const uint32 ls2Id) -> bool
{
    return (hasLS1 && ls1Id == linkshellId) || (hasLS2 && ls2Id == linkshellId);
}

// InScopeForZoneWide is true for Say/Shout kinds (zone-wide delivery).
inline auto InScopeForZoneWide(const GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND kind) -> bool
{
    return kind == GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Say ||
           kind == GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Shout;
}

// InScope combines kind dispatch for NominateProposal::inScope.
inline auto InScope(
    const GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND kind,
    const uint32                             partyId,
    const uint32                             allianceId,
    const uint32                             linkshellId,
    const bool                               hasParty,
    const uint32                             memberPartyId,
    const bool                               hasAlliance,
    const uint32                             memberAllianceId,
    const bool                               hasLS1,
    const uint32                             ls1Id,
    const bool                               hasLS2,
    const uint32                             ls2Id) -> bool
{
    switch (kind)
    {
        case GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Party:
            return InScopeForParty(partyId, allianceId, hasParty, memberPartyId, hasAlliance, memberAllianceId);
        case GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Linkshell1:
        case GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Linkshell2:
            return InScopeForLinkshell(linkshellId, hasLS1, ls1Id, hasLS2, ls2Id);
        case GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Say:
        case GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND::Shout:
            return InScopeForZoneWide(kind);
    }

    return false;
}

} // namespace nominatehelpers
