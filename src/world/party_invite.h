#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandlePartyInvite(const ipc::PartyInvite& message, Reroute&& reroute)
{
    std::invoke(reroute, message.inviteeId, message);
}

} // namespace worldipc
