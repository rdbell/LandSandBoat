#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace mapipc
{

template <typename Lookup, typename Update>
void HandleCharVarUpdate(const ipc::CharVarUpdate& message, Lookup&& lookup, Update&& update)
{
    if (auto* character = std::invoke(lookup, message.charId))
    {
        std::invoke(update, character, message.varName, message.value, message.expiry);
    }
}

} // namespace mapipc
