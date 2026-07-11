#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandlePartyInviteResponse(const ipc::PartyInviteResponse& message, Reroute&& reroute)
{
    std::invoke(reroute, message.inviterId, message);
}

} // namespace worldipc
