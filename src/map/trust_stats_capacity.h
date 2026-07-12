#pragma once

#include <algorithm>
#include <cstdint>

// Pure LoadTrustStatsAndSkills policy tails.
// Parity: internal/trustutils ScaleToGrade / ClampAlterEgoMultiplier /
// ApplySkillMultiplier / scaleSubJobStat (slice 1611).

namespace truststatshelpers
{

// ALTER_EGO multiplier clamp range (default 1.0 outside range).
constexpr float MultiplierMin = 0.1f;
constexpr float MultiplierMax = 2.0f;
constexpr float MultiplierDef = 1.0f;

// ENABLE_TRUST_ALTER_EGO_EXPO mod pins.
constexpr std::int16_t ExpoHPP       = 50;
constexpr std::int16_t ExpoMPP       = 50;
constexpr std::int16_t ExpoStatusRes = 25;

// Subjob base-stat floor: sLvl > 15 → /2 else 0.
constexpr std::uint8_t SubJobStatLevelFloor = 15;

// GetMaxSkill level arg: mLvl > 99 ? 99 : mLvl.
constexpr auto SkillCapLevel(const std::uint8_t mLvl) -> std::uint8_t
{
    return mLvl > 99 ? 99 : mLvl;
}

// Unsigned mapRanges lambda (inputVal may wrap when below start).
constexpr auto MapRanges(const unsigned inputStart, const unsigned inputEnd, const unsigned outputStart, const unsigned outputEnd,
                         const unsigned inputVal) -> unsigned
{
    const unsigned inputRange  = inputEnd - inputStart;
    const unsigned outputRange = outputEnd - outputStart;
    const unsigned output      = (inputVal - inputStart) * outputRange / inputRange + outputStart;
    return std::clamp(output, outputStart, outputEnd);
}

// HPscale/MPscale (~0.7–1.4) → grade ranks 1–7.
inline auto ScaleToGrade(const float input) -> std::uint8_t
{
    const auto multipliedInput    = static_cast<unsigned>(input * 100.0f);
    const auto reverseMappedGrade = MapRanges(70U, 140U, 1U, 7U, multipliedInput);
    return static_cast<std::uint8_t>(std::clamp(7U - reverseMappedGrade, 1U, 7U));
}

inline auto ClampAlterEgoMultiplier(const float mult) -> float
{
    return (mult >= MultiplierMin && mult <= MultiplierMax) ? mult : MultiplierDef;
}

// Trust subjob HP sum before ALTER_EGO mult (no /2 — unlike PC ComposeSubJobHP).
inline auto ComposeTrustSubJobHP(const float base, const float to60, const float over30, const std::uint8_t slvl, const std::int32_t subLevelOver30,
                                 const std::int32_t subLevelOver10) -> float
{
    return base + (to60 * static_cast<float>(slvl - 1)) + (over30 * static_cast<float>(subLevelOver30)) + static_cast<float>(subLevelOver30) +
           static_cast<float>(subLevelOver10);
}

inline auto FinalTrustMaxHP(const float raceStat, const float jobStat, const std::int32_t bonusStat, const float sJobStat, const float mult) -> std::int16_t
{
    return static_cast<std::int16_t>((raceStat + jobStat + static_cast<float>(bonusStat) + sJobStat) * mult);
}

// Trust sJob MP quirk: base + to60 only (no *(slvl-1)).
inline auto ComposeTrustSubJobMP(const float base, const float to60) -> float
{
    return base + to60;
}

inline auto FinalTrustMaxMP(const float raceStat, const float jobStat, const float sJobStat, const float mult) -> std::int16_t
{
    return static_cast<std::int16_t>((raceStat + jobStat + sJobStat) * mult);
}

// sLvl > 15 → base/2; else 0.
constexpr auto ScaleSubJobStat(const std::uint16_t base, const std::uint8_t sLvl) -> std::uint16_t
{
    if (sLvl > SubJobStatLevelFloor)
    {
        return static_cast<std::uint16_t>(base / 2);
    }
    return 0;
}

inline auto FinalTrustStat(const std::uint16_t family, const std::uint16_t mainJob, const std::uint16_t subJob, const float mult) -> std::uint16_t
{
    return static_cast<std::uint16_t>((family + mainJob + subJob) * mult);
}

// Skill mult has no clamp (matches C++).
inline auto ApplySkillMultiplier(const std::uint16_t maxSkill, const float mult) -> std::uint16_t
{
    return static_cast<std::uint16_t>(static_cast<float>(maxSkill) * mult);
}

} // namespace truststatshelpers
