#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleMessageSystem(const ipc::MessageSystem& message, Reroute&& reroute)
{
    std::invoke(reroute, message.recipientId, message);
}

} // namespace worldipc
