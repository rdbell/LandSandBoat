#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename RerouteChar, typename RerouteZone>
void HandleEntityInformationRequest(const ipc::EntityInformationRequest& message, RerouteChar&& rerouteChar, RerouteZone&& rerouteZone)
{
    if (message.entityType == 0x01)
    {
        std::invoke(rerouteChar, message.targetId, message);
        return;
    }

    const auto zoneId = static_cast<uint16>((message.targetId >> 12) & 0x0FFF);
    std::invoke(rerouteZone, zoneId, message);
}

} // namespace worldipc
