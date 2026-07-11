#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleLinkshellRankChange(const ipc::LinkshellRankChange& message, Reroute&& reroute)
{
    std::invoke(reroute, message.memberName, message);
}

} // namespace worldipc
