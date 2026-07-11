#pragma once

#include "common/ipc_structs.h"
#include "enums/msg_std.h"

#include <cstddef>
#include <functional>
#include <utility>

namespace mapipc
{

struct PartyInviteResponseInviterState
{
    bool hasParty{};
    bool isPartyLeader{};
    bool hasAlliance{};
};

// CanJoinExistingAlliance reports whether an inviter's alliance still has room
// for another party. LSB requires a positive party count strictly less than 3.
inline auto CanJoinExistingAlliance(const std::size_t alliancePartyCount) -> bool
{
    return alliancePartyCount > 0 && alliancePartyCount < 3;
}

// bothLeadersCount returns the accounts_parties row count for the dual-leader
// query used by the accept path. alliancePartyCount returns the distinct party
// count for the inviter's alliance. inviteeInParty reports whether the invitee
// already has a non-zero partyid row.
template <typename Lookup, typename Inspect, typename BothLeadersCount, typename AlliancePartyCount, typename InviteeInParty,
          typename NotifyDeclined, typename AddToAlliance, typename CreateAlliance, typename EnsureParty, typename AddMember,
          typename FeedbackStandard>
void HandlePartyInviteResponse(const ipc::PartyInviteResponse& message, Lookup&& lookup, Inspect&& inspect,
                               BothLeadersCount&& bothLeadersCount, AlliancePartyCount&& alliancePartyCount,
                               InviteeInParty&& inviteeInParty, NotifyDeclined&& notifyDeclined, AddToAlliance&& addToAlliance,
                               CreateAlliance&& createAlliance, EnsureParty&& ensureParty, AddMember&& addMember,
                               FeedbackStandard&& feedbackStandard)
{
    auto* inviter = std::invoke(lookup, message.inviterId);
    if (!inviter)
    {
        return;
    }

    if (message.inviteAnswer == 0)
    {
        std::invoke(notifyDeclined, inviter);
        return;
    }

    if (std::invoke(bothLeadersCount, message.inviterId, message.inviteeId) == 2)
    {
        auto state = std::invoke(inspect, inviter);
        if (!state.hasParty)
        {
            std::invoke(feedbackStandard, ipc::MessageStandard{
                                              .recipientId = message.inviteeId,
                                              .message     = MsgStd::CannotBeProcessed,
                                          });
            return;
        }

        if (state.hasAlliance)
        {
            if (CanJoinExistingAlliance(std::invoke(alliancePartyCount, message.inviterId)))
            {
                std::invoke(addToAlliance, inviter, message.inviteeId);
            }
            else
            {
                std::invoke(feedbackStandard, ipc::MessageStandard{
                                                  .recipientId = message.inviteeId,
                                                  .message     = MsgStd::CannotBeProcessed,
                                              });
            }
            return;
        }

        std::invoke(createAlliance, inviter, message.inviteeId);
        return;
    }

    std::invoke(ensureParty, inviter);
    const auto state = std::invoke(inspect, inviter);
    if (state.hasParty && state.isPartyLeader && !std::invoke(inviteeInParty, message.inviteeId))
    {
        std::invoke(addMember, inviter, message.inviteeId);
    }
}

} // namespace mapipc
