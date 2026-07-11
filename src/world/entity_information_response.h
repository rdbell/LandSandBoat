#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleEntityInformationResponse(const ipc::EntityInformationResponse& message, Reroute&& reroute)
{
    std::invoke(reroute, message.requesterId, message);
}

} // namespace worldipc
