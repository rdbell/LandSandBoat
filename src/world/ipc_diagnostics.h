#pragma once

#include "common/ipp.h"

#include <fmt/format.h>
#include <cstddef>
#include <cstdint>
#include <string>

namespace worldipc
{
inline auto FormatEmptyStructWarning(const IPP& source) -> std::string
{
    return fmt::format("Received EmptyStruct message from {} - this is probably a bug", source.toString());
}

inline auto FormatUnknownMessageWarning(const IPP& source, uint8_t code, std::size_t size) -> std::string
{
    return fmt::format("Received unknown message from {} with code {} and size {}", source.toString(), code, size);
}
} // namespace worldipc
