#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandlePartyDisband(const ipc::PartyDisband& message, Reroute&& reroute)
{
    std::invoke(reroute, message.partyId, message);
}

} // namespace worldipc
