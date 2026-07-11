#pragma once

#include "common/cbasetypes.h"

#include <functional>

namespace worldipc
{

template <typename Message, typename Lookup, typename Send>
void RerouteMessageToPartyMembers(const uint32 partyId, const Message& message, Lookup&& lookup, Send&& send)
{
    const auto endpoints = std::invoke(lookup, partyId);
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
