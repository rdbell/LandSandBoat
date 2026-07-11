#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Lookup, typename Send>
void HandleChatMessageLinkshell(const ipc::ChatMessageLinkshell& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup, message.linkshellId);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
