#include "test_map_party_invite_response_1303.h"

#include "map/party_invite_response.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

struct FakeInviter
{
    bool hasParty{};
    bool isLeader{};
    bool hasAlliance{};

    int    declines{};
    int    addToAlliance{};
    int    createAlliance{};
    int    ensureParty{};
    int    addMember{};
    uint32 lastInvitee{};
};

struct Recording
{
    std::vector<ipc::MessageStandard> standards{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map party invite response 1303 self-test failed: " << label << '\n';
    }
    return condition;
}

auto inspect(FakeInviter* inviter) -> mapipc::PartyInviteResponseInviterState
{
    return {
        .hasParty      = inviter->hasParty,
        .isPartyLeader = inviter->isLeader,
        .hasAlliance   = inviter->hasAlliance,
    };
}

auto runResponse(const ipc::PartyInviteResponse& message, FakeInviter* inviter, const std::size_t bothLeaders,
                 const std::size_t allianceParties, const bool inviteeInParty, Recording& recording) -> void
{
    mapipc::HandlePartyInviteResponse(
        message,
        [inviter](const uint32)
        {
            return inviter;
        },
        inspect,
        [bothLeaders](const uint32, const uint32)
        {
            return bothLeaders;
        },
        [allianceParties](const uint32)
        {
            return allianceParties;
        },
        [inviteeInParty](const uint32)
        {
            return inviteeInParty;
        },
        [](FakeInviter* target)
        {
            ++target->declines;
        },
        [](FakeInviter* target, const uint32 inviteeId)
        {
            ++target->addToAlliance;
            target->lastInvitee = inviteeId;
        },
        [](FakeInviter* target, const uint32 inviteeId)
        {
            ++target->createAlliance;
            target->lastInvitee = inviteeId;
            target->hasParty    = true;
            target->hasAlliance = true;
            target->isLeader    = true;
        },
        [](FakeInviter* target)
        {
            ++target->ensureParty;
            if (!target->hasParty)
            {
                target->hasParty = true;
                target->isLeader = true;
            }
        },
        [](FakeInviter* target, const uint32 inviteeId)
        {
            ++target->addMember;
            target->lastInvitee = inviteeId;
        },
        [&](const ipc::MessageStandard& feedback)
        {
            recording.standards.push_back(feedback);
        });
}

auto testCanJoinExistingAlliance() -> bool
{
    return expect(!mapipc::CanJoinExistingAlliance(0), "zero parties blocks") &&
           expect(mapipc::CanJoinExistingAlliance(1), "one party allows") &&
           expect(mapipc::CanJoinExistingAlliance(2), "two parties allow") &&
           expect(!mapipc::CanJoinExistingAlliance(3), "three parties full");
}

auto testDeclineAndMissing() -> bool
{
    FakeInviter inviter{};
    Recording   recording{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 11, .inviterId = std::numeric_limits<uint32>::max(), .inviteAnswer = 0 }, &inviter, 0, 0,
                false, recording);

    int missingSideEffects{};
    mapipc::HandlePartyInviteResponse(
        ipc::PartyInviteResponse{ .inviteAnswer = 1 },
        [](const uint32) -> FakeInviter*
        {
            return nullptr;
        },
        [](FakeInviter*)
        {
            return mapipc::PartyInviteResponseInviterState{};
        },
        [&](const uint32, const uint32)
        {
            ++missingSideEffects;
            return 0;
        },
        [&](const uint32)
        {
            ++missingSideEffects;
            return 0;
        },
        [&](const uint32)
        {
            ++missingSideEffects;
            return false;
        },
        [&](FakeInviter*)
        {
            ++missingSideEffects;
        },
        [&](FakeInviter*, const uint32)
        {
            ++missingSideEffects;
        },
        [&](FakeInviter*, const uint32)
        {
            ++missingSideEffects;
        },
        [&](FakeInviter*)
        {
            ++missingSideEffects;
        },
        [&](FakeInviter*, const uint32)
        {
            ++missingSideEffects;
        },
        [&](const ipc::MessageStandard&)
        {
            ++missingSideEffects;
        });

    return expect(inviter.declines == 1 && inviter.addMember == 0 && recording.standards.empty(), "decline notifies inviter only") &&
           expect(missingSideEffects == 0, "missing inviter complete no-op");
}

auto testBothLeadersAllianceAndCreate() -> bool
{
    FakeInviter allianceInviter{ .hasParty = true, .isLeader = true, .hasAlliance = true };
    Recording   allianceOk{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 22, .inviterId = 33, .inviteAnswer = 1 }, &allianceInviter, 2, 2, false, allianceOk);

    FakeInviter allianceFull{ .hasParty = true, .isLeader = true, .hasAlliance = true };
    Recording   allianceFullRec{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 44, .inviterId = 55, .inviteAnswer = 1 }, &allianceFull, 2, 3, false, allianceFullRec);

    FakeInviter createAlliance{ .hasParty = true, .isLeader = true, .hasAlliance = false };
    Recording   createRec{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 66, .inviterId = 77, .inviteAnswer = 1 }, &createAlliance, 2, 0, false, createRec);

    FakeInviter missingLocalParty{};
    Recording   missingPartyRec{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 88, .inviterId = 99, .inviteAnswer = 1 }, &missingLocalParty, 2, 0, false, missingPartyRec);

    return expect(allianceInviter.addToAlliance == 1 && allianceInviter.lastInvitee == 22 && allianceOk.standards.empty(), "join alliance") &&
           expect(allianceFull.addToAlliance == 0 && allianceFullRec.standards.size() == 1 &&
                      allianceFullRec.standards[0].recipientId == 44 && allianceFullRec.standards[0].message == MsgStd::CannotBeProcessed,
                  "full alliance rejects") &&
           expect(createAlliance.createAlliance == 1 && createAlliance.lastInvitee == 66, "create alliance") &&
           expect(missingLocalParty.createAlliance == 0 && missingPartyRec.standards.size() == 1 &&
                      missingPartyRec.standards[0].message == MsgStd::CannotBeProcessed,
                  "both-leaders without local party rejects");
}

auto testPartyJoinPath() -> bool
{
    FakeInviter noParty{};
    Recording   rec{};
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 101, .inviterId = 102, .inviteAnswer = 1 }, &noParty, 0, 0, false, rec);

    FakeInviter leader{ .hasParty = true, .isLeader = true };
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 103, .inviterId = 104, .inviteAnswer = 1 }, &leader, 1, 0, false, rec);

    FakeInviter leaderInviteeInParty{ .hasParty = true, .isLeader = true };
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 105, .inviterId = 106, .inviteAnswer = 1 }, &leaderInviteeInParty, 0, 0, true, rec);

    FakeInviter notLeader{ .hasParty = true, .isLeader = false };
    runResponse(ipc::PartyInviteResponse{ .inviteeId = 107, .inviterId = 108, .inviteAnswer = 1 }, &notLeader, 0, 0, false, rec);

    return expect(noParty.ensureParty == 1 && noParty.addMember == 1 && noParty.lastInvitee == 101, "create party then add") &&
           expect(leader.ensureParty == 1 && leader.addMember == 1 && leader.lastInvitee == 103, "leader adds member") &&
           expect(leaderInviteeInParty.addMember == 0, "invitee already in party skips add") &&
           expect(notLeader.addMember == 0, "non-leader skips add") &&
           expect(rec.standards.empty(), "party join path has no feedback");
}

} // namespace

auto runMapPartyInviteResponse1303SelfTests() -> bool
{
    return testCanJoinExistingAlliance() && testDeclineAndMissing() && testBothLeadersAllianceAndCreate() && testPartyJoinPath();
}
