#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Lookup, typename Send>
void HandleChatMessageServerMessage(const ipc::ChatMessageServerMessage& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
