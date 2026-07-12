#pragma once

#include <cmath>
#include <cstdint>

// Pure petutils::LoadJugStats growth / HP / MP / stat blend.
// Parity: internal/petutils jug_stats.go (slice 1602).

namespace jugstatshelpers
{

// JOB_* pins used by IsJugCasterJob.
constexpr std::uint8_t JobPLD = 7;
constexpr std::uint8_t JobWHM = 3;
constexpr std::uint8_t JobBLM = 4;
constexpr std::uint8_t JobRDM = 5;
constexpr std::uint8_t JobDRK = 8;
constexpr std::uint8_t JobBLU = 16;
constexpr std::uint8_t JobSCH = 20;

constexpr auto JugGrowth(const std::uint8_t level) -> float
{
    if (level > 75)
    {
        return 1.22f;
    }
    if (level > 65)
    {
        return 1.20f;
    }
    if (level > 55)
    {
        return 1.18f;
    }
    if (level > 50)
    {
        return 1.16f;
    }
    if (level > 45)
    {
        return 1.12f;
    }
    if (level > 35)
    {
        return 1.09f;
    }
    if (level > 25)
    {
        return 1.07f;
    }
    return 1.0f;
}

inline auto JugMaxHP(const std::uint8_t level, const float hpScale) -> std::int16_t
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<std::int16_t>(17.0 * std::pow(static_cast<double>(level), static_cast<double>(JugGrowth(level))) * static_cast<double>(hpScale));
}

inline auto JugMaxMP(const std::uint8_t level, const float mpScale) -> std::int16_t
{
    if (level == 0)
    {
        return 0;
    }
    return static_cast<std::int16_t>(15.2 * std::pow(static_cast<double>(level), 1.1075) * static_cast<double>(mpScale));
}

constexpr auto IsJugCasterJob(const std::uint8_t mJob) -> bool
{
    switch (mJob)
    {
        case JobPLD:
        case JobWHM:
        case JobBLM:
        case JobRDM:
        case JobDRK:
        case JobBLU:
        case JobSCH:
            return true;
        default:
            return false;
    }
}

constexpr auto JugStatBlend(const std::uint16_t familyRank, const std::uint16_t jobGrade) -> std::uint16_t
{
    return static_cast<std::uint16_t>((familyRank + jobGrade) * 0.9f);
}

} // namespace jugstatshelpers
