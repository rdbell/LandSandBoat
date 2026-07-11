#pragma once

#include "common/ipc_structs.h"

#include <fmt/format.h>

#include <functional>
#include <optional>
#include <string>
#include <utility>

namespace mapipc
{

// FormatLuaFunctionError mirrors the ShowError format string used by
// IPCClient::handleMessage_LuaFunction when safe_script fails.
inline auto FormatLuaFunctionError(const std::string& error, const std::string& funcString) -> std::string
{
    return fmt::format("IPCClient::handleMessage_LuaFunction: error: {}: {}", error, funcString);
}

// execute returns nullopt on success or a human-readable error string on
// failure. logError receives the fully formatted diagnostic when execution
// fails. Requester/executor zone IDs are intentionally unused today; LSB has
// a TODO for returning values to requesterZoneId.
template <typename Execute, typename LogError>
void HandleLuaFunction(const ipc::LuaFunction& message, Execute&& execute, LogError&& logError)
{
    if (auto error = std::invoke(execute, message.funcString))
    {
        std::invoke(logError, FormatLuaFunctionError(*error, message.funcString));
    }
}

} // namespace mapipc
