#pragma once

#include <cmath>
#include <cstdint>

// Pure automaton weapon damage: floor((skill / 8.7) * 2 + 3).
// Parity: internal/automaton WeaponDamage (slice 1592).
// Production: petutils LoadAutomatonStats setDamage; battle_entity main/ranged.

namespace automatonweapondamagehelpers
{

constexpr float SkillDivisor = 8.7f;

inline auto WeaponDamage(const std::uint16_t skill) -> std::uint16_t
{
    return static_cast<std::uint16_t>(std::floor((static_cast<float>(skill) / SkillDivisor) * 2.0f + 3.0f));
}

// Live dmg rating path: base + MAIN_DMG_RATING / RANGED_DMG_RATING (no clamp).
inline auto WeaponDamageWithRating(const std::uint16_t skill, const std::int32_t dmgRating) -> std::int32_t
{
    return static_cast<std::int32_t>(WeaponDamage(skill)) + dmgRating;
}

} // namespace automatonweapondamagehelpers
