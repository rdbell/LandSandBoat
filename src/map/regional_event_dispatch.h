#pragma once

#include "common/ipc_structs.h"

#include <functional>
#include <span>
#include <utility>

namespace mapipc
{

// HandleConquestEvent forwards the raw conquest message type and payload span
// to the map conquest subsystem exactly once. Payload bytes are not copied by
// this seam; the consumer receives the same range as IPCClient.
template <typename Consume>
void HandleConquestEvent(const ipc::ConquestEvent& message, Consume&& consume)
{
    std::invoke(consume, message.type, std::span<const uint8>{ message.payload.data(), message.payload.size() });
}

// The three remaining regional map IPC handlers are currently empty in LSB.
// These seams exist so typed frame ownership can pin the intentional no-ops.
template <typename... Args>
void HandleBesiegedEvent(const ipc::BesiegedEvent&, Args&&...)
{
}

template <typename... Args>
void HandleCampaignEvent(const ipc::CampaignEvent&, Args&&...)
{
}

template <typename... Args>
void HandleColonizationEvent(const ipc::ColonizationEvent&, Args&&...)
{
}

} // namespace mapipc
