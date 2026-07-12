#pragma once

#include <algorithm>
#include <cstdint>

// Pure FinalizePetStatistics policy tails.
// Parity: internal/petutils finalize_stats.go (slice 1607).

namespace finalizepetstatshelpers
{

// Rank arg to GetMaxSkill(rank, level) for base C magic evasion.
constexpr std::uint8_t MEVASkillRank = 7;

// Clamp pet main level for the MEVA skill-table lookup.
constexpr auto MEVALevel(const std::uint8_t petMLvl) -> std::uint8_t
{
    return std::min<std::uint8_t>(99, petMLvl);
}

// Compose setModifier MEVA from rank skill + existing Mod::MEVA.
constexpr auto ComposeMEVA(const std::int16_t rankSkill, const std::int16_t existingMEVA) -> std::int16_t
{
    return static_cast<std::int16_t>(rankSkill + existingMEVA);
}

// Stout Servant pet DMG add: -(traitValue * 100).
constexpr auto StoutServantDamage(const std::int16_t traitValue) -> std::int16_t
{
    return static_cast<std::int16_t>(-(traitValue * 100));
}

} // namespace finalizepetstatshelpers
