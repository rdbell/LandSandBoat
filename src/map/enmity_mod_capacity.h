#pragma once

#include <cstdint>

// Pure GetEnmityModDamage / GetEnmityModCure.
// Parity: internal/enmity EnmityModDamage / EnmityModCure.

namespace enmitymodhelpers
{

// GetEnmityModDamage: level * 31 / 50 + 6 (integer division toward zero).
constexpr auto GetEnmityModDamage(const std::int16_t level) -> std::int32_t
{
    return static_cast<std::int32_t>(level) * 31 / 50 + 6;
}

// GetEnmityModCure level bands.
inline auto GetEnmityModCure(const std::int16_t level) -> std::int32_t
{
    if (level <= 10)
    {
        return static_cast<std::int32_t>(level) + 10;
    }
    if (level <= 50)
    {
        return 20 + (static_cast<std::int32_t>(level) - 10) / 2;
    }
    // int16(40 + (level-50)*0.6) then widen
    return static_cast<std::int16_t>(40 + (static_cast<std::int16_t>(level) - 50) * 0.6);
}

} // namespace enmitymodhelpers
