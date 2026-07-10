#pragma once

#include "common/ipc_structs.h"

#include <functional>
#include <utility>

namespace mapipc
{

template <typename Lookup, typename Refresh>
void HandleGMCallResponse(const ipc::GMCallResponse& message, Lookup&& lookup, Refresh&& refresh)
{
    if (auto* character = std::invoke(std::forward<Lookup>(lookup), message.charId))
    {
        std::invoke(std::forward<Refresh>(refresh), character);
    }
}

} // namespace mapipc
