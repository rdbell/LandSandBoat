#pragma once

#include <algorithm>
#include <cstdint>

// Pure CalculateJugPetStats level policy.
// Parity: internal/petutils jug_level.go (slice 1606).

namespace juglevelhelpers
{

// Main weapon delay / base delay pin for jug pets.
constexpr std::uint16_t WeaponDelay = 240;

// Cap catalog max (+ merits) to greater of master main level and mainhand iLvl.
constexpr auto CapLevel(const std::uint8_t masterMLvl, const std::uint8_t mainhandILvl) -> std::uint8_t
{
    return masterMLvl > mainhandILvl ? masterMLvl : mainhandILvl;
}

// Apply master-level / iLvl cap after beast-affinity raise.
constexpr auto CapHighestLevel(const std::uint8_t highestLvl, const std::uint8_t capLevel) -> std::uint8_t
{
    return highestLvl > capLevel ? capLevel : highestLvl;
}

// Random span width: 3 - clamp(JUG_LEVEL_RANGE, 0, 2) → rolls in [0, width).
constexpr auto LevelRangeWidth(const std::int16_t jugLevelRangeMod) -> std::uint8_t
{
    const auto clamped = std::clamp<std::int16_t>(jugLevelRangeMod, 0, 2);
    return static_cast<std::uint8_t>(3 - clamped);
}

// Subtract random offset then min with spawn level.
constexpr auto FinalJugLevel(const std::uint8_t highestAfterRandom, const std::uint8_t spawnLevel) -> std::uint8_t
{
    return highestAfterRandom < spawnLevel ? highestAfterRandom : spawnLevel;
}

// highestLvl after random: if random >= highest, floor at 0 (unsigned subtract would wrap in naive C++).
// Production: highestLvl -= random with uint8 (wraps). Mirror LSB uint8 subtract.
constexpr auto ApplyLevelRandom(const std::uint8_t highestLvl, const std::uint8_t randomOffset) -> std::uint8_t
{
    return static_cast<std::uint8_t>(highestLvl - randomOffset);
}

// Raise catalog max by beast-affinity merit value (uint8 sum, no cap here).
constexpr auto RaiseWithBeastAffinity(const std::uint8_t catalogMaxLevel, const std::uint8_t beastAffinityMerit) -> std::uint8_t
{
    const auto sum = static_cast<unsigned>(catalogMaxLevel) + static_cast<unsigned>(beastAffinityMerit);
    return sum > 255 ? 255 : static_cast<std::uint8_t>(sum);
}

} // namespace juglevelhelpers
