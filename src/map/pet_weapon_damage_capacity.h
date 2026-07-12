#pragma once

#include <cmath>
#include <cstdint>

// Pure jug / wyvern pet weapon damage formulas.
// Parity: internal/petutils JugWeaponDamage / WyvernWeaponDamage (slice 1593).

namespace petweapondamagehelpers
{

// GetJugWeaponDamage: level * (level < 40 ? 1.4 - level/100 : 1)
inline auto JugWeaponDamage(const float mainLevel) -> std::uint16_t
{
    return static_cast<std::uint16_t>(mainLevel * (mainLevel < 40.0f ? 1.4 - mainLevel / 100.0 : 1.0));
}

// Wyvern main/ranged base: floor(level / 2) + 3
constexpr auto WyvernWeaponDamage(const std::uint8_t level) -> std::uint16_t
{
    return static_cast<std::uint16_t>(level / 2) + 3;
}

inline auto WyvernWeaponDamageWithRating(const std::uint8_t level, const std::int32_t dmgRating) -> std::int32_t
{
    return static_cast<std::int32_t>(WyvernWeaponDamage(level)) + dmgRating;
}

// Jug live path on battle_entity: JugWeaponDamage(level) + MAIN_DMG_RATING
inline auto JugWeaponDamageWithRating(const float mainLevel, const std::int32_t dmgRating) -> std::int32_t
{
    return static_cast<std::int32_t>(JugWeaponDamage(mainLevel)) + dmgRating;
}

} // namespace petweapondamagehelpers
