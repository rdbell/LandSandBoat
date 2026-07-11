#pragma once
#include "common/ipc_structs.h"
#include <functional>
namespace worldipc
{
template <typename Reroute>
void HandleAssistChannelEvent(const ipc::AssistChannelEvent& message, Reroute&& reroute)
{
    std::invoke(reroute, message.receiverId, message);
}
} // namespace worldipc
