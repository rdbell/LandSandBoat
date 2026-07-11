#pragma once

#include "common/cbasetypes.h"

#include <array>
#include <cstddef>

namespace charautomatonhelpers
{

template <std::size_t Size>
inline auto ApplyElementalCapacityBonus(
    const uint8 bonus,
    uint8& currentBonus,
    std::array<uint8, Size>& elementMax) -> bool
{
    if (bonus == currentBonus)
    {
        return false;
    }

    const int8 difference = static_cast<int8>(bonus) - currentBonus;
    for (auto& capacity : elementMax)
    {
        capacity += difference;
    }
    currentBonus = bonus;
    return true;
}

template <std::size_t Size>
inline auto HasAttachment(const std::array<uint8, Size>& attachments, const uint8 attachment) -> bool
{
    for (const auto attachmentID : attachments)
    {
        if (attachmentID == attachment)
        {
            return true;
        }
    }
    return false;
}

} // namespace charautomatonhelpers
