#pragma once

#include <cstdint>
#include <vector>

namespace automatoncontrollerstatusremovaldecode
{
inline auto Decode(std::uint32_t packed) -> std::vector<std::uint16_t>
{
    std::vector<std::uint16_t> removes;
    while (packed > 0)
    {
        removes.emplace_back(static_cast<std::uint16_t>(packed & 0xFF));
        packed >>= 8;
    }
    return removes;
}
} // namespace automatoncontrollerstatusremovaldecode
