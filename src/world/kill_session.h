#pragma once

#include "common/ipc_structs.h"
#include "kill_session_zones.h"

#include <cstdint>
#include <functional>

namespace worldipc
{

template <typename LookupZones, typename LookupPreviousEndpoint, typename AllEndpoints, typename Send>
void HandleKillSession(const ipc::KillSession& message,
                       LookupZones&& lookupZones,
                       LookupPreviousEndpoint&& lookupPreviousEndpoint,
                       AllEndpoints&& allEndpoints,
                       Send&& send)
{
    if (const auto zones = std::invoke(lookupZones, message.victimId))
    {
        if (zones->previous != zones->current)
        {
            const auto endpoint = std::invoke(lookupPreviousEndpoint, static_cast<std::uint16_t>(zones->previous));
            std::invoke(send, endpoint, message);
        }
        return;
    }

    for (const auto& endpoint : std::invoke(allEndpoints))
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
