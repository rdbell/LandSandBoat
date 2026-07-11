#pragma once

#include "common/cbasetypes.h"

#include <array>
#include <cstddef>
#include <optional>

namespace charstoragehelpers
{

template <typename T, std::size_t Size>
inline auto Resolve(const uint8 locationId, const std::array<T*, Size>& containers) -> std::optional<T*>
{
    if (locationId >= Size)
    {
        return std::nullopt;
    }
    return containers[locationId];
}

} // namespace charstoragehelpers
