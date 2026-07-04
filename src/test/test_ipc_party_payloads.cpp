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

#include "test_ipc_party_payloads.h"

#include "common/ipc_structs.h"
#include "map/enums/party_kind.h"

#include <iostream>
#include <string>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC party payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC party payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testPartyKinds() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(static_cast<int>(PartyKind::Party), 0, "PartyKind::Party") && ok;
    ok      = expectEqualInt(static_cast<int>(PartyKind::Alliance), 5, "PartyKind::Alliance") && ok;
    return ok;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const ipc::PartyInvite invite{};
    ok = expectEqualInt(invite.inviteeId, 0, "invite invitee id default") && ok;
    ok = expectEqualInt(invite.inviteeTargId, 0, "invite invitee target default") && ok;
    ok = expectEqualInt(invite.inviterId, 0, "invite inviter id default") && ok;
    ok = expectEqualInt(invite.inviterTargId, 0, "invite inviter target default") && ok;
    ok = expectEqualString(invite.inviterName, "", "invite name default") && ok;
    ok = expectEqualInt(static_cast<int>(invite.inviteType), static_cast<int>(PartyKind::Party), "invite type default") && ok;

    const ipc::PartyInviteResponse response{};
    ok = expectEqualInt(response.inviteeId, 0, "response invitee id default") && ok;
    ok = expectEqualInt(response.inviteeTargId, 0, "response invitee target default") && ok;
    ok = expectEqualInt(response.inviterId, 0, "response inviter id default") && ok;
    ok = expectEqualInt(response.inviterTargId, 0, "response inviter target default") && ok;
    ok = expectEqualInt(response.inviteAnswer, 0, "response answer default") && ok;

    ok = expectEqualInt(ipc::PartyReload{}.partyId, 0, "party reload default") && ok;
    ok = expectEqualInt(ipc::PartyDisband{}.partyId, 0, "party disband default") && ok;
    ok = expectEqualInt(ipc::AllianceReload{}.allianceId, 0, "alliance reload default") && ok;
    ok = expectEqualInt(ipc::AllianceDissolve{}.allianceId, 0, "alliance dissolve default") && ok;
    ok = expectEqualInt(ipc::PlayerKick{}.victimId, 0, "player kick default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    const ipc::PartyInvite invite{
        .inviteeId     = 1001,
        .inviteeTargId = 22,
        .inviterId     = 2002,
        .inviterTargId = 33,
        .inviterName   = "Leader",
        .inviteType    = PartyKind::Alliance,
    };
    ok = expectEqualInt(invite.inviteeId, 1001, "assigned invitee id") && ok;
    ok = expectEqualInt(invite.inviteeTargId, 22, "assigned invitee target") && ok;
    ok = expectEqualInt(invite.inviterId, 2002, "assigned inviter id") && ok;
    ok = expectEqualInt(invite.inviterTargId, 33, "assigned inviter target") && ok;
    ok = expectEqualString(invite.inviterName, "Leader", "assigned inviter name") && ok;
    ok = expectEqualInt(static_cast<int>(invite.inviteType), static_cast<int>(PartyKind::Alliance), "assigned invite type") && ok;

    const ipc::PartyInviteResponse response{ .inviteeId = 1001, .inviteeTargId = 22, .inviterId = 2002, .inviterTargId = 33, .inviteAnswer = 1 };
    ok = expectEqualInt(response.inviteeId, 1001, "assigned response invitee id") && ok;
    ok = expectEqualInt(response.inviteeTargId, 22, "assigned response invitee target") && ok;
    ok = expectEqualInt(response.inviterId, 2002, "assigned response inviter id") && ok;
    ok = expectEqualInt(response.inviterTargId, 33, "assigned response inviter target") && ok;
    ok = expectEqualInt(response.inviteAnswer, 1, "assigned response answer") && ok;

    ok = expectEqualInt(ipc::PartyReload{ .partyId = 3003 }.partyId, 3003, "assigned party reload") && ok;
    ok = expectEqualInt(ipc::PartyDisband{ .partyId = 3004 }.partyId, 3004, "assigned party disband") && ok;
    ok = expectEqualInt(ipc::AllianceReload{ .allianceId = 4004 }.allianceId, 4004, "assigned alliance reload") && ok;
    ok = expectEqualInt(ipc::AllianceDissolve{ .allianceId = 4005 }.allianceId, 4005, "assigned alliance dissolve") && ok;
    ok = expectEqualInt(ipc::PlayerKick{ .victimId = 5005 }.victimId, 5005, "assigned player kick") && ok;

    return ok;
}

} // namespace

auto runIPCPartyPayloadSelfTests() -> bool
{
    return testPartyKinds() && testDefaultPayloads() && testAssignedPayloads();
}
