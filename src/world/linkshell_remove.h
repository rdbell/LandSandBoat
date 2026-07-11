#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleLinkshellRemove(const ipc::LinkshellRemove& message, Reroute&& reroute)
{
    std::invoke(reroute, message.victimName, message);
}

} // namespace worldipc
