#pragma once

#include "common/ipc_structs.h"
#include "regional_event_dispatch.h"

namespace worldipc
{

template <typename Handle>
void HandleBesiegedEvent(const IPP& source, const ipc::BesiegedEvent& message, Handle&& handle)
{
    HandleRegionalEvent(source, message, std::forward<Handle>(handle));
}

} // namespace worldipc
