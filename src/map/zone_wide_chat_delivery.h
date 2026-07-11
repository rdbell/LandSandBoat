#pragma once

#include "common/ipc_structs.h"

#include <functional>
#include <utility>

namespace mapipc
{

inline auto ShouldDeliverYell(const bool zoneAllowsYell, const uint32 recipientId, const uint32 senderId) -> bool
{
    return zoneAllowsYell && recipientId != senderId;
}

inline auto ShouldDeliverAssist(const bool zoneAllowsAssist, const uint32 recipientId, const uint32 senderId, const bool eligible) -> bool
{
    return zoneAllowsAssist && recipientId != senderId && eligible;
}

inline auto ShouldDeliverServerMessage(const uint32 recipientId, const uint32 senderId, const bool skipSender) -> bool
{
    return !skipSender || recipientId != senderId;
}

// forEachCandidate invokes visit(zoneAllowsYell, recipientId, character) for every
// live character on this map process, including those in zones that disable yell.
template <typename ForEachCandidate, typename Deliver>
void HandleChatMessageYell(const ipc::ChatMessageYell& message, ForEachCandidate&& forEachCandidate, Deliver&& deliver)
{
    std::invoke(
        forEachCandidate,
        [&](const bool zoneAllowsYell, const uint32 recipientId, auto* character)
        {
            if (character && ShouldDeliverYell(zoneAllowsYell, recipientId, message.senderId))
            {
                std::invoke(deliver, character, message);
            }
        });
}

// forEachCandidate invokes visit(zoneAllowsAssist, recipientId, eligible, character).
template <typename ForEachCandidate, typename Deliver>
void HandleChatMessageAssist(const ipc::ChatMessageAssist& message, ForEachCandidate&& forEachCandidate, Deliver&& deliver)
{
    std::invoke(
        forEachCandidate,
        [&](const bool zoneAllowsAssist, const uint32 recipientId, const bool eligible, auto* character)
        {
            if (character && ShouldDeliverAssist(zoneAllowsAssist, recipientId, message.senderId, eligible))
            {
                std::invoke(deliver, character, message);
            }
        });
}

// forEachCandidate invokes visit(recipientId, character) for every live character.
template <typename ForEachCandidate, typename Deliver>
void HandleChatMessageServerMessage(const ipc::ChatMessageServerMessage& message, ForEachCandidate&& forEachCandidate, Deliver&& deliver)
{
    std::invoke(
        forEachCandidate,
        [&](const uint32 recipientId, auto* character)
        {
            if (character && ShouldDeliverServerMessage(recipientId, message.senderId, message.skipSender))
            {
                std::invoke(deliver, character, message);
            }
        });
}

} // namespace mapipc
