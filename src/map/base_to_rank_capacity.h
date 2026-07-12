#pragma once

#include <cstdint>

// Pure mobutils/petutils GetBaseToRank (integer A–G rank tables).
// Parity: internal/mobutils GetBaseToRank (slice 1594 production wire).

namespace basetorankhelpers
{

// rank 1–7 (A–G); other ranks return 0.
constexpr auto GetBaseToRank(const std::uint8_t rank, const std::uint16_t lvl) -> std::uint16_t
{
    switch (rank)
    {
        case 1:
            return static_cast<std::uint16_t>(5 + ((lvl - 1) * 50) / 100); // A
        case 2:
            return static_cast<std::uint16_t>(4 + ((lvl - 1) * 45) / 100); // B
        case 3:
            return static_cast<std::uint16_t>(4 + ((lvl - 1) * 40) / 100); // C
        case 4:
            return static_cast<std::uint16_t>(3 + ((lvl - 1) * 35) / 100); // D
        case 5:
            return static_cast<std::uint16_t>(3 + ((lvl - 1) * 30) / 100); // E
        case 6:
            return static_cast<std::uint16_t>(2 + ((lvl - 1) * 25) / 100); // F
        case 7:
            return static_cast<std::uint16_t>(2 + ((lvl - 1) * 20) / 100); // G
        default:
            return 0;
    }
}

} // namespace basetorankhelpers
