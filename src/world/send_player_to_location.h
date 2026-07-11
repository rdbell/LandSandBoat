#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleSendPlayerToLocation(const ipc::SendPlayerToLocation& message, Reroute&& reroute)
{
    std::invoke(reroute, message.targetId, message);
}

} // namespace worldipc
