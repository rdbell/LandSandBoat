#pragma once

#include <cstdint>

// Pure petutils::GetJugBase rank×level contribution.
// Parity: internal/petutils JugBase (slice 1594 production wire).

namespace jugbasehelpers
{

// rank 1–7 (A–G); other ranks return 0.
// level > 50 uses high band; level ≤ 50 uses low band (strict > 50).
inline auto JugBase(const std::uint8_t rank, const std::uint8_t level) -> std::uint16_t
{
    if (level > 50)
    {
        const float delta = static_cast<float>(level - 50);
        switch (rank)
        {
            case 1:
                return static_cast<std::uint16_t>(153 + delta * 5.0f);
            case 2:
                return static_cast<std::uint16_t>(147 + delta * 4.9f);
            case 3:
                return static_cast<std::uint16_t>(136 + delta * 4.8f);
            case 4:
                return static_cast<std::uint16_t>(126 + delta * 4.7f);
            case 5:
                return static_cast<std::uint16_t>(116 + delta * 4.5f);
            case 6:
                return static_cast<std::uint16_t>(106 + delta * 4.4f);
            case 7:
                return static_cast<std::uint16_t>(96 + delta * 4.3f);
            default:
                return 0;
        }
    }

    // level==0: avoid uint8 underflow of (level-1); live pets use level ≥ 1.
    const float delta = level == 0 ? 0.0f : static_cast<float>(level - 1);
    switch (rank)
    {
        case 1:
            return static_cast<std::uint16_t>(6 + delta * 3.0f);
        case 2:
            return static_cast<std::uint16_t>(5 + delta * 2.9f);
        case 3:
            return static_cast<std::uint16_t>(5 + delta * 2.8f);
        case 4:
            return static_cast<std::uint16_t>(4 + delta * 2.7f);
        case 5:
            return static_cast<std::uint16_t>(4 + delta * 2.5f);
        case 6:
            return static_cast<std::uint16_t>(3 + delta * 2.4f);
        case 7:
            return static_cast<std::uint16_t>(3 + delta * 2.3f);
        default:
            return 0;
    }
}

} // namespace jugbasehelpers
