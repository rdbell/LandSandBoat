#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Remove, typename Lookup, typename Update, typename Reroute>
void HandleCharZone(const ipc::CharZone& message, Remove&& remove, Lookup&& lookup, Update&& update, Reroute&& reroute)
{
    if (message.destinationZoneId == 0xFFFF)
    {
        std::invoke(remove, message.charId);
        return;
    }

    const auto cachedEndpoint = std::invoke(lookup, message.destinationZoneId);
    if (!cachedEndpoint)
    {
        return;
    }
    std::invoke(update, message.charId, *cachedEndpoint);

    std::invoke(reroute, message.destinationZoneId, message);
}

} // namespace worldipc
