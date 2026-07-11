#pragma once

#include "common/blowfish.h"
#include "common/ipc_structs.h"

#include <functional>
#include <utility>

namespace mapipc
{

// ShouldDestroyKillSession reports whether a looked-up session must be closed
// for a remote KillSession notification. Only BLOWFISH_PENDING_ZONE sessions
// are destroyed; every other status is intentionally left alone.
inline auto ShouldDestroyKillSession(const BLOWFISH status) -> bool
{
    return status == BLOWFISH_PENDING_ZONE;
}

// HandleKillSession inspects both the active and pending session tables for
// the victim. Each present session is destroyed only when its blowfish status
// is BLOWFISH_PENDING_ZONE. Missing lookups are independent no-ops.
template <typename ActiveLookup, typename PendingLookup, typename StatusOf, typename DestroyActive, typename DestroyPending>
void HandleKillSession(const ipc::KillSession& message, ActiveLookup&& activeLookup, PendingLookup&& pendingLookup,
                       StatusOf&& statusOf, DestroyActive&& destroyActive, DestroyPending&& destroyPending)
{
    if (auto session = std::invoke(activeLookup, message.victimId))
    {
        if (ShouldDestroyKillSession(std::invoke(statusOf, session)))
        {
            std::invoke(destroyActive, session);
        }
    }

    if (auto session = std::invoke(pendingLookup, message.victimId))
    {
        if (ShouldDestroyKillSession(std::invoke(statusOf, session)))
        {
            std::invoke(destroyPending, session);
        }
    }
}

} // namespace mapipc
