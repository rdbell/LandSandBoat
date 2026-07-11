#pragma once

#include "common/ipc_structs.h"
#include "regional_event_dispatch.h"

namespace worldipc
{

template <typename Handle>
void HandleColonizationEvent(const IPP& source, const ipc::ColonizationEvent& message, Handle&& handle)
{
    HandleRegionalEvent(source, message, std::forward<Handle>(handle));
}

} // namespace worldipc
