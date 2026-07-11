#pragma once

#include "common/ipc_structs.h"
#include "common/ipp_message.h"

#include <functional>

namespace worldipc
{

template <typename Handle>
void HandleConquestEvent(const IPP& source, const ipc::ConquestEvent& message, Handle&& handle)
{
    std::invoke(handle, message.type, IPPMessage{ source, message.payload });
}

} // namespace worldipc
