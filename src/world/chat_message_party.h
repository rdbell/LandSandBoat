#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Lookup, typename Send>
void HandleChatMessageParty(const ipc::ChatMessageParty& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup, message.partyId);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
