#pragma once

#include "common/ipc_structs.h"
#include "common/ipp.h"

#include <functional>

namespace worldipc
{

template <typename Lookup, typename Forward, typename Offline>
void HandleChatMessageTell(const IPP& source, const ipc::ChatMessageTell& message, Lookup&& lookup, Forward&& forward, Offline&& offline)
{
    const auto endpoint = std::invoke(lookup, message.recipientName);
    if (!endpoint)
    {
        std::invoke(offline, source, ipc::MessageStandard{
                                         .recipientId = message.senderId,
                                         .message     = MsgStd::TellNotReceivedOffline,
                                     });
        return;
    }

    std::invoke(forward, *endpoint, message);
}

} // namespace worldipc
