#pragma once

#include <cmath>
#include <cstdint>

// Pure CalculateMobStats product tails (HP combine, settings mult, MP, stats).
// Parity: internal/mobutils calculate_stats_product (slice 1620).

namespace mobstatsproducthelpers
{

constexpr float PetHPScale = 0.30f;

// JOBTYPE pins used by hasMp job switches.
constexpr std::uint8_t JobPLD = 7;
constexpr std::uint8_t JobWHM = 3;
constexpr std::uint8_t JobBLM = 4;
constexpr std::uint8_t JobRDM = 5;
constexpr std::uint8_t JobDRK = 8;
constexpr std::uint8_t JobBLU = 16;
constexpr std::uint8_t JobSCH = 20;
constexpr std::uint8_t JobSMN = 15;

inline auto ClampSettingsMultiplier(const float v) -> float
{
    if (v >= 0.1f && v <= 2.0f)
    {
        return v;
    }
    return 1.0f;
}

inline auto CombineMobHP(const std::uint32_t baseMobHP, const std::uint32_t sjHP, const bool hasMaster) -> std::uint32_t
{
    auto mobHP = baseMobHP + sjHP;
    if (hasMaster)
    {
        mobHP = static_cast<std::uint32_t>(static_cast<float>(mobHP) * PetHPScale);
    }
    return mobHP;
}

inline auto ApplyHPMultiplier(const std::int32_t maxHP, const float multiplier) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(maxHP) * ClampSettingsMultiplier(multiplier));
}

inline auto JobGivesMP(const std::uint8_t job) -> bool
{
    switch (job)
    {
        case JobPLD:
        case JobWHM:
        case JobBLM:
        case JobRDM:
        case JobDRK:
        case JobBLU:
        case JobSCH:
        case JobSMN:
            return true;
        default:
            return false;
    }
}

inline auto JobHasMP(const std::uint8_t mJob, const std::uint8_t sJob) -> bool
{
    return JobGivesMP(mJob) || JobGivesMP(sJob);
}

inline auto ResolveMPScale(const float mpScale, const std::int16_t mpBaseMod) -> float
{
    if (mpBaseMod != 0)
    {
        return static_cast<float>(mpBaseMod) / 100.0f;
    }
    return mpScale;
}

// (int16)(18.2 * pow(mLvl, 1.1075) * scale) + 10
inline auto CalculateMobMaxMP(const std::uint8_t mLvl, const float scale) -> std::int32_t
{
    const auto inner = 18.2 * std::pow(static_cast<double>(mLvl), 1.1075) * static_cast<double>(scale);
    return static_cast<std::int32_t>(static_cast<std::int16_t>(inner)) + 10;
}

inline auto ApplyMPMultiplier(const std::int32_t maxMP, const float multiplier) -> std::int32_t
{
    return static_cast<std::int32_t>(static_cast<float>(maxMP) * ClampSettingsMultiplier(multiplier));
}

inline auto SumStat(const std::uint16_t family, const std::uint16_t main, const std::uint16_t sub) -> std::uint16_t
{
    return static_cast<std::uint16_t>(family + main + sub);
}

inline auto ApplyStatMultiplier(const std::uint16_t stat, const float multiplier) -> std::uint16_t
{
    return static_cast<std::uint16_t>(static_cast<float>(stat) * ClampSettingsMultiplier(multiplier));
}

inline auto HalveSubJobStat(const std::uint16_t stat) -> std::uint16_t
{
    return static_cast<std::uint16_t>(stat / 2);
}

} // namespace mobstatsproducthelpers
