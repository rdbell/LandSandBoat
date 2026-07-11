#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Lookup, typename Send>
void HandleChatMessageUnity(const ipc::ChatMessageUnity& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup, message.unityLeaderId);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
