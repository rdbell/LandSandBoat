#pragma once

#include "common/cbasetypes.h"

#include <functional>

namespace worldipc
{

template <typename Message, typename Lookup, typename Send>
void RerouteMessageToCharId(const uint32 charId, const Message& message, Lookup&& lookup, Send&& send)
{
    const auto endpoint = std::invoke(lookup, charId);
    if (!endpoint)
    {
        return;
    }
    std::invoke(send, *endpoint, message);
}

} // namespace worldipc
