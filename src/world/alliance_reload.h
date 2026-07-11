#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleAllianceReload(const ipc::AllianceReload& message, Reroute&& reroute)
{
    std::invoke(reroute, message.allianceId, message);
}

} // namespace worldipc
