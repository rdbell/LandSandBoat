#pragma once

#include "common/ipc_structs.h"

#include <functional>

namespace worldipc
{

template <typename Endpoints, typename Send>
void HandleAccountLogin(const ipc::AccountLogin& message, const Endpoints& endpoints, Send&& send)
{
    for (const auto& endpoint : endpoints)
    {
        std::invoke(send, endpoint, message);
    }
}

} // namespace worldipc
