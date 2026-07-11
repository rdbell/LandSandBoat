#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace mapipc
{

template <typename Lookup, typename Touch, typename Create>
void HandleCharZone(const ipc::CharZone& message, Lookup&& lookup, Touch&& touch, Create&& create)
{
    if (auto* session = std::invoke(lookup, message.charId))
    {
        std::invoke(touch, session);
        return;
    }
    std::invoke(create, message.charId);
}

} // namespace mapipc
