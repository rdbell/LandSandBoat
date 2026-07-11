#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

namespace gmcall::detail
{

auto PersistCall(uint32_t charId, std::string_view message) -> uint32_t;

struct PendingResponse
{
    uint32_t    callId;
    std::string response;
};

auto OldestPendingResponse(uint32_t charId) -> std::optional<PendingResponse>;
void AcknowledgeOldestResponse(uint32_t charId);

} // namespace gmcall::detail
