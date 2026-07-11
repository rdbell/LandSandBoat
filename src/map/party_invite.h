#pragma once

#include "common/ipc_structs.h"
#include "enums/msg_std.h"
#include "enums/party_kind.h"

#include <functional>
#include <utility>

namespace mapipc
{

enum class PartyInviteDecision : uint8_t
{
    CannotInvite = 0,
    Blockaid     = 1,
    LevelSync    = 2,
    Accept       = 3,
};

struct PartyInviteeSnapshot
{
    bool dead{};
    bool inPrison{};
    bool invitePending{};
    bool hasParty{};
    bool isPartyLeader{};
    bool hasAlliance{};
    bool blockingAid{};
    bool hasLevelSync{};
};

// EvaluatePartyInvite mirrors the ordered reject gates in
// IPCClient::handleMessage_PartyInvite before InvitePending is set.
inline auto EvaluatePartyInvite(const PartyKind inviteType, const PartyInviteeSnapshot& state) -> PartyInviteDecision
{
    if (state.dead ||
        state.inPrison ||
        state.invitePending ||
        (state.hasParty && inviteType == PartyKind::Party) ||
        (inviteType == PartyKind::Alliance && (!state.hasParty || !state.isPartyLeader || state.hasAlliance)))
    {
        return PartyInviteDecision::CannotInvite;
    }

    if (state.blockingAid)
    {
        return PartyInviteDecision::Blockaid;
    }

    if (state.hasLevelSync)
    {
        return PartyInviteDecision::LevelSync;
    }

    return PartyInviteDecision::Accept;
}

template <typename Lookup, typename Inspect, typename FeedbackStandard, typename FeedbackSystem, typename NotifyBlockaid, typename Accept>
void HandlePartyInvite(const ipc::PartyInvite& message, Lookup&& lookup, Inspect&& inspect, FeedbackStandard&& feedbackStandard,
                       FeedbackSystem&& feedbackSystem, NotifyBlockaid&& notifyBlockaid, Accept&& accept)
{
    auto* invitee = std::invoke(lookup, message.inviteeId);
    if (!invitee)
    {
        return;
    }

    const auto decision = EvaluatePartyInvite(message.inviteType, std::invoke(inspect, invitee));
    switch (decision)
    {
        case PartyInviteDecision::CannotInvite:
            std::invoke(feedbackStandard, ipc::MessageStandard{
                                              .recipientId = message.inviterId,
                                              .message     = MsgStd::CannotInvite,
                                          });
            return;

        case PartyInviteDecision::Blockaid:
            std::invoke(feedbackSystem, ipc::MessageSystem{
                                            .recipientId = message.inviterId,
                                            .message     = MsgStd::TargetIsCurrentlyBlocking,
                                        });
            std::invoke(notifyBlockaid, invitee);
            std::invoke(feedbackStandard, ipc::MessageStandard{
                                              .recipientId = message.inviterId,
                                              .message     = MsgStd::CannotInvite,
                                          });
            return;

        case PartyInviteDecision::LevelSync:
            std::invoke(feedbackStandard, ipc::MessageStandard{
                                              .recipientId = message.inviterId,
                                              .message     = MsgStd::CannotInviteLevelSync,
                                          });
            return;

        case PartyInviteDecision::Accept:
            std::invoke(accept, invitee, message);
            return;
    }
}

} // namespace mapipc
