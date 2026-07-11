#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace mapipc
{

template <typename Lookup>
void HandlePlayerKick(const ipc::PlayerKick& message, Lookup&& lookup)
{
    auto* player = std::invoke(lookup, message.victimId);
    if (player)
    {
        // The victim is already absent from party/alliance storage, so force a direct update.
        player->ReloadPartyInc();
    }
}

} // namespace mapipc
