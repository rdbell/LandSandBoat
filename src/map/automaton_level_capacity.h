#pragma once

#include <cstdint>

// Pure CalculateAutomatonStats level policy.
// Parity: internal/automaton level.go (slice 1609).

namespace automatonlevelhelpers
{

// JOB_PUP from battle_entity JOBTYPE.
constexpr std::uint8_t JobPUP = 18;

// Main level: PUP main uses masterMLvl + AUTOMATON_LVL_BONUS; else master SLevel.
constexpr auto MainLevel(const bool pupMain, const std::uint8_t masterMLvl, const std::uint8_t masterSLvl, const std::int16_t automatonLvlBonus) -> std::uint8_t
{
    if (!pupMain)
    {
        return masterSLvl;
    }
    const auto sum = static_cast<int>(masterMLvl) + static_cast<int>(automatonLvlBonus);
    if (sum < 0)
    {
        return 0;
    }
    if (sum > 255)
    {
        return 255;
    }
    return static_cast<std::uint8_t>(sum);
}

// Sub level is mainLevel / 2 (integer divide).
constexpr auto SubLevel(const std::uint8_t mainLevel) -> std::uint8_t
{
    return mainLevel / 2;
}

} // namespace automatonlevelhelpers
