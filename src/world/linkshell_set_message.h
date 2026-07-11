#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleLinkshellSetMessage(const ipc::LinkshellSetMessage& message, Reroute&& reroute)
{
    std::invoke(reroute, message.linkshellId, message);
}

} // namespace worldipc
