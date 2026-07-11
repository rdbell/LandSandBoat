#pragma once

#include "common/ipc_structs.h"
#include "regional_event_dispatch.h"

namespace worldipc
{

template <typename Handle>
void HandleCampaignEvent(const IPP& source, const ipc::CampaignEvent& message, Handle&& handle)
{
    HandleRegionalEvent(source, message, std::forward<Handle>(handle));
}

} // namespace worldipc
