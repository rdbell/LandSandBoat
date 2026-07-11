#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleAllianceDissolve(const ipc::AllianceDissolve& message, Reroute&& reroute)
{
    std::invoke(reroute, message.allianceId, message);
}

} // namespace worldipc
