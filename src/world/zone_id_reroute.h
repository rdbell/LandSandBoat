#pragma once

#include <cstdint>
#include <functional>

namespace worldipc
{

template <typename Message, typename Lookup, typename Send>
void RerouteMessageToZoneId(std::uint16_t zoneId, const Message& message, Lookup&& lookup, Send&& send)
{
    const auto endpoint = std::invoke(lookup, zoneId);
    if (!endpoint)
    {
        return;
    }
    std::invoke(send, *endpoint, message);
}

} // namespace worldipc
