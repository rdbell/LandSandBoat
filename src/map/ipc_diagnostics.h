#pragma once

#include "common/ipp.h"

#include <fmt/format.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace mapipc
{

// FormatEmptyStructWarning mirrors IPCClient::handleMessage_EmptyStruct.
inline auto FormatEmptyStructWarning(const IPP& source) -> std::string
{
    return fmt::format("Received EmptyStruct message from {} - this is probably a bug", source.toString());
}

// FormatUnknownMessageWarning mirrors IPCClient::handleUnknownMessage.
inline auto FormatUnknownMessageWarning(const IPP& source, const uint8_t code, const std::size_t size) -> std::string
{
    return fmt::format("Received unknown message from {} with code {} and size {}", source.toString(), code, size);
}

} // namespace mapipc
