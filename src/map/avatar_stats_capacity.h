#pragma once

#include <cstdint>

// Pure LoadAvatarStats policy tails (race pin, astral HP, jobStat×1.5, EVA).
// Shared HP/MP/stat band composition lives in calculate_stats_capacity.h.
// Parity: internal/petutils avatar_stats.go (slice 1603).

namespace avatarstatshelpers
{

// Fixed grade race row index used by LoadAvatarStats (LSB race = 3).
// Comment in petutils says "Tarutaru" but the value is used as GetRaceGrades index as-is.
constexpr std::uint8_t RaceGradeGroup = 3;

// PETID pins (petutils.h).
constexpr std::uint32_t PetIDAlexander = 17;
constexpr std::uint32_t PetIDOdin      = 18;

// Flat maxhp bonus for Odin / Alexander.
constexpr std::int32_t AstralHPBonus = 6800;

// Main-job base-stat multiplier (comment: assuming BLM/BLM for avatars).
constexpr float JobStatMultiplier = 1.5f;

constexpr auto IsAstralPet(const std::uint32_t petID) -> bool
{
    return petID == PetIDOdin || petID == PetIDAlexander;
}

// Adds AstralHPBonus when pet is Odin or Alexander.
constexpr auto ApplyAstralHPBonus(const std::int32_t bonusStat, const std::uint32_t petID) -> std::int32_t
{
    if (IsAstralPet(petID))
    {
        return bonusStat + AstralHPBonus;
    }
    return bonusStat;
}

// jobStat * 1.5f for avatar base STR–CHR.
inline auto ScaleJobStat(const float jobStat) -> float
{
    return jobStat * JobStatMultiplier;
}

// Evasion skill → Mod::EVA: identity ≤200, else 200 + (skill-200)*0.9.
inline auto EvasionFromSkill(const std::int16_t evaskill) -> std::int16_t
{
    if (evaskill > 200)
    {
        return static_cast<std::int16_t>(200 + (evaskill - 200) * 0.9f);
    }
    return evaskill;
}

} // namespace avatarstatshelpers
