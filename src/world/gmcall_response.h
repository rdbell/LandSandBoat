#pragma once

#include "common/ipc_structs.h"

#include <cstddef>

namespace world::gmcall
{

constexpr std::size_t MaxResponseBytes = 1024;

inline auto TruncateResponse(ipc::GMCallResponse message) -> ipc::GMCallResponse
{
    message.message = message.message.substr(0, MaxResponseBytes);
    return message;
}

} // namespace world::gmcall
