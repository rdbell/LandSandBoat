#pragma once

#include "common/ipc_structs.h"

#include <fmt/format.h>

#include <string>

namespace world::gmcall
{

inline auto FormatRequestLog(const ipc::GMCallRequest& message) -> std::string
{
    return fmt::format("GM Call #{} from {} (charId: {}, accId: {}, zone: {}): {}",
                       message.callId,
                       message.charName,
                       message.charId,
                       message.accId,
                       message.zoneId,
                       message.message);
}

} // namespace world::gmcall
