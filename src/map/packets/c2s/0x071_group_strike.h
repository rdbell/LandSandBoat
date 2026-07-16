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

#pragma once

#include "base.h"

enum class GP_CLI_COMMAND_GROUP_STRIKE_KIND : uint8_t
{
    Party      = 0,
    Linkshell1 = 1,
    Linkshell2 = 2,
    Alliance   = 5,
};

// GROUP_STRIKE's local party/alliance graph mutations. Database and IPC work
// for cross-process victims remains in the packet host.
namespace groupstrikehelpers
{

enum class AllianceAction : uint8
{
    None,
    RemoveParty,
    Dissolve,
};

struct MutationPlan
{
    AllianceAction allianceAction = AllianceAction::None;
    bool           removeVictim   = false;
};

// MakeLocalPartyPlan selects the local PARTY branch. A local victim is always
// removed; kicking oneself from a one-member alliance party first removes that
// party or dissolves its singleton alliance.
[[nodiscard]] auto MakeLocalPartyPlan(
    bool localVictim,
    bool victimIsRequester,
    bool hasAlliance,
    bool partyHasOnlyOneMember,
    bool allianceHasOnlyOneParty) -> MutationPlan;

// MakeLocalAlliancePlan selects the local ALLIANCE branch. Only the victim
// themselves or the main-party leader targeting another party leader can
// remove a party from an alliance.
[[nodiscard]] auto MakeLocalAlliancePlan(
    bool localVictim,
    bool victimInAlliance,
    bool victimIsRequester,
    bool requesterIsMainParty,
    bool victimIsPartyLeader,
    bool allianceHasOnlyOneParty) -> MutationPlan;

} // namespace groupstrikehelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0071
// This packet is sent by the client when kicking a member from a party, alliance or linkshell.
GP_CLI_PACKET(GP_CLI_COMMAND_GROUP_STRIKE,
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint16_t ActIndex;  // PS2: ActIndex
              uint8_t  Kind;      // PS2: Kind
              uint8_t  padding00; // PS2: dammy2
              uint8_t  sName[15]; // PS2: sName
);
