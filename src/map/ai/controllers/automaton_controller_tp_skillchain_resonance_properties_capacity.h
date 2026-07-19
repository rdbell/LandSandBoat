#pragma once

#include <array>
#include <cstdint>

namespace automatoncontrollertpskillchainresonanceproperties
{
inline auto Decode(std::uint16_t power) -> std::array<std::uint16_t, 3>
{
    return { static_cast<std::uint16_t>(power & 0xF), static_cast<std::uint16_t>((power >> 4) & 0xF), static_cast<std::uint16_t>(power >> 8) };
}
} // namespace automatoncontrollertpskillchainresonanceproperties
