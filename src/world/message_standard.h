#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Reroute>
void HandleMessageStandard(const ipc::MessageStandard& message, Reroute&& reroute)
{
    std::invoke(reroute, message.recipientId, message);
}

} // namespace worldipc
