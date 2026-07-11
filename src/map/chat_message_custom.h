#pragma once

#include "common/ipc_structs.h"

#include <functional>
#include <utility>

namespace mapipc
{

// CanReceiveCustomChat mirrors the live-recipient gate used by
// IPCClient::handleMessage_ChatMessageCustom: disappeared and imprisoned
// characters never receive the packet.
inline auto CanReceiveCustomChat(const bool disappeared, const bool inPrison) -> bool
{
    return !disappeared && !inPrison;
}

template <typename Lookup, typename Inspect, typename Deliver>
void HandleChatMessageCustom(const ipc::ChatMessageCustom& message, Lookup&& lookup, Inspect&& inspect, Deliver&& deliver)
{
    auto* character = std::invoke(lookup, message.recipientId);
    if (!character)
    {
        return;
    }

    const auto [disappeared, inPrison] = std::invoke(inspect, character);
    if (CanReceiveCustomChat(disappeared, inPrison))
    {
        std::invoke(deliver, character, message);
    }
}

} // namespace mapipc
