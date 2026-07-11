#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleLuaFunction(const ipc::LuaFunction& message, Reroute&& reroute)
{
    std::invoke(reroute, message.executorZoneId, message);
}

} // namespace worldipc
