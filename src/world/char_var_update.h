#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleCharVarUpdate(const ipc::CharVarUpdate& message, Reroute&& reroute)
{
    std::invoke(reroute, message.charId, message);
}

} // namespace worldipc
