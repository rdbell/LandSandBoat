#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace mapipc
{

template <typename Lookup, typename DeliverString, typename DeliverParameters>
void HandleMessageStandard(
    const ipc::MessageStandard& message,
    Lookup&&                    lookup,
    DeliverString&&             deliverString,
    DeliverParameters&&         deliverParameters)
{
    auto* player = std::invoke(lookup, message.recipientId);
    if (!player)
    {
        return;
    }

    if (!message.string2.empty() && message.param0 == 0 && message.param1 == 0)
    {
        std::invoke(deliverString, player, message.string2, message.message);
    }
    else
    {
        std::invoke(deliverParameters, player, message.param0, message.param1, message.message);
    }
}

template <typename Lookup, typename DeliverParameters>
void HandleMessageSystem(const ipc::MessageSystem& message, Lookup&& lookup, DeliverParameters&& deliverParameters)
{
    auto* player = std::invoke(lookup, message.recipientId);
    if (!player)
    {
        return;
    }

    std::invoke(deliverParameters, player, message.param0, message.param1, message.message);
}

} // namespace mapipc
