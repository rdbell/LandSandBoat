#include "test_map_party_invite_1300.h"

#include "map/party_invite.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

struct FakeInvitee
{
    mapipc::PartyInviteeSnapshot state{};
    int                          accepts{};
    int                          blockaidNotices{};
    uint32                       pendingId{};
    uint16                       pendingTargId{};
};

struct Recording
{
    std::vector<ipc::MessageStandard> standards{};
    std::vector<ipc::MessageSystem>   systems{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map party invite 1300 self-test failed: " << label << '\n';
    }
    return condition;
}

auto runInvite(const ipc::PartyInvite& message, FakeInvitee* invitee, Recording& recording) -> void
{
    mapipc::HandlePartyInvite(
        message,
        [invitee](const uint32)
        {
            return invitee;
        },
        [](FakeInvitee* target)
        {
            return target->state;
        },
        [&](const ipc::MessageStandard& feedback)
        {
            recording.standards.push_back(feedback);
        },
        [&](const ipc::MessageSystem& feedback)
        {
            recording.systems.push_back(feedback);
        },
        [](FakeInvitee* target)
        {
            ++target->blockaidNotices;
        },
        [](FakeInvitee* target, const ipc::PartyInvite& invite)
        {
            ++target->accepts;
            target->pendingId     = invite.inviterId;
            target->pendingTargId = invite.inviterTargId;
        });
}

auto testEvaluateMatrix() -> bool
{
    using mapipc::EvaluatePartyInvite;
    using mapipc::PartyInviteDecision;
    using mapipc::PartyInviteeSnapshot;

    const PartyInviteeSnapshot clean{};
    const bool                 acceptParty = EvaluatePartyInvite(PartyKind::Party, clean) == PartyInviteDecision::Accept;
    const bool                 acceptAlliance =
        EvaluatePartyInvite(PartyKind::Alliance, PartyInviteeSnapshot{ .hasParty = true, .isPartyLeader = true }) == PartyInviteDecision::Accept;

    const bool deadRejects = EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .dead = true }) == PartyInviteDecision::CannotInvite;
    const bool prisonRejects =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .inPrison = true }) == PartyInviteDecision::CannotInvite;
    const bool pendingRejects =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .invitePending = true }) == PartyInviteDecision::CannotInvite;
    const bool partyMemberRejects =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .hasParty = true }) == PartyInviteDecision::CannotInvite;
    const bool allianceNoPartyRejects = EvaluatePartyInvite(PartyKind::Alliance, clean) == PartyInviteDecision::CannotInvite;
    const bool allianceNotLeaderRejects =
        EvaluatePartyInvite(PartyKind::Alliance, PartyInviteeSnapshot{ .hasParty = true, .isPartyLeader = false }) ==
        PartyInviteDecision::CannotInvite;
    const bool allianceHasAllianceRejects =
        EvaluatePartyInvite(PartyKind::Alliance, PartyInviteeSnapshot{ .hasParty = true, .isPartyLeader = true, .hasAlliance = true }) ==
        PartyInviteDecision::CannotInvite;
    const bool blockaid =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .blockingAid = true }) == PartyInviteDecision::Blockaid;
    const bool levelSync =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .hasLevelSync = true }) == PartyInviteDecision::LevelSync;
    // Basic reject takes precedence over blockaid/level-sync.
    const bool basicBeforeBlockaid =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .dead = true, .blockingAid = true }) == PartyInviteDecision::CannotInvite;
    const bool blockaidBeforeLevelSync =
        EvaluatePartyInvite(PartyKind::Party, PartyInviteeSnapshot{ .blockingAid = true, .hasLevelSync = true }) ==
        PartyInviteDecision::Blockaid;

    return expect(acceptParty && acceptAlliance, "accept paths") &&
           expect(deadRejects && prisonRejects && pendingRejects && partyMemberRejects, "basic cannot-invite") &&
           expect(allianceNoPartyRejects && allianceNotLeaderRejects && allianceHasAllianceRejects, "alliance cannot-invite") &&
           expect(blockaid && levelSync, "secondary rejects") &&
           expect(basicBeforeBlockaid && blockaidBeforeLevelSync, "decision precedence");
}

auto testHandlerEffects() -> bool
{
    const ipc::PartyInvite message{
        .inviteeId     = std::numeric_limits<uint32>::max(),
        .inviteeTargId = 0xABCD,
        .inviterId     = 0x01020304,
        .inviterTargId = 0x1122,
        .inviterName   = "Inviter",
        .inviteType    = PartyKind::Alliance,
    };

    FakeInvitee acceptInvitee{ .state = { .hasParty = true, .isPartyLeader = true } };
    Recording   acceptRecording{};
    runInvite(message, &acceptInvitee, acceptRecording);

    FakeInvitee cannotInvitee{ .state = { .dead = true } };
    Recording   cannotRecording{};
    runInvite(message, &cannotInvitee, cannotRecording);

    // Blockaid with Party type (no party membership required before the blockaid gate).
    FakeInvitee blockaidClean{ .state = { .blockingAid = true } };
    Recording   blockaidRecording{};
    runInvite(ipc::PartyInvite{ .inviterId = 7, .inviteType = PartyKind::Party }, &blockaidClean, blockaidRecording);

    FakeInvitee levelSyncInvitee{ .state = { .hasLevelSync = true } };
    Recording   levelSyncRecording{};
    runInvite(ipc::PartyInvite{ .inviterId = 9, .inviteType = PartyKind::Party }, &levelSyncInvitee, levelSyncRecording);

    int missingCalls{};
    mapipc::HandlePartyInvite(
        message,
        [](const uint32) -> FakeInvitee*
        {
            return nullptr;
        },
        [&](FakeInvitee*)
        {
            ++missingCalls;
            return mapipc::PartyInviteeSnapshot{};
        },
        [&](const ipc::MessageStandard&)
        {
            ++missingCalls;
        },
        [&](const ipc::MessageSystem&)
        {
            ++missingCalls;
        },
        [&](FakeInvitee*)
        {
            ++missingCalls;
        },
        [&](FakeInvitee*, const ipc::PartyInvite&)
        {
            ++missingCalls;
        });

    return expect(acceptInvitee.accepts == 1 && acceptInvitee.pendingId == message.inviterId && acceptInvitee.pendingTargId == message.inviterTargId,
                  "accept sets pending from inviter fields") &&
           expect(acceptRecording.standards.empty() && acceptRecording.systems.empty(), "accept no feedback") &&
           expect(cannotRecording.standards.size() == 1 && cannotRecording.standards[0].recipientId == message.inviterId &&
                      cannotRecording.standards[0].message == MsgStd::CannotInvite,
                  "cannot-invite feedback") &&
           expect(blockaidClean.blockaidNotices == 1 && blockaidRecording.systems.size() == 1 &&
                      blockaidRecording.systems[0].message == MsgStd::TargetIsCurrentlyBlocking &&
                      blockaidRecording.standards.size() == 1 && blockaidRecording.standards[0].message == MsgStd::CannotInvite,
                  "blockaid triple side effects") &&
           expect(levelSyncRecording.standards.size() == 1 && levelSyncRecording.standards[0].message == MsgStd::CannotInviteLevelSync,
                  "level-sync feedback") &&
           expect(missingCalls == 0, "missing invitee is complete no-op");
}

} // namespace

auto runMapPartyInvite1300SelfTests() -> bool
{
    return testEvaluateMatrix() && testHandlerEffects();
}
