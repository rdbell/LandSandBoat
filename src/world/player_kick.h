#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandlePlayerKick(const ipc::PlayerKick& message, Reroute&& reroute)
{
    std::invoke(reroute, message.victimId, message);
}

} // namespace worldipc
