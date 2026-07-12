#pragma once

#include <algorithm>
#include <cstdint>
#include <utility>

// Pure CalculateStats policy from charutils (HP/MP/base-stat composition).

namespace calculatestatshelpers
{

// Scale column indices used by grade::GetHPScale / GetMPScale / GetStatScale.
constexpr std::int32_t BaseValueColumn   = 0;
constexpr std::int32_t ScaleTo60Column   = 1;
constexpr std::int32_t ScaleOver30Column = 2;
constexpr std::int32_t ScaleOver60Column = 3; // HP over-60-to-75 column
constexpr std::int32_t ScaleOver75Column = 4; // HP over-75 column
constexpr std::int32_t MPScaleOver60     = 2; // MP/stat over-60 column
constexpr std::int32_t StatScaleOver75   = 3; // Stat over-75 column

// JOB_WAR / JOB_MON from battle_entity JOBTYPE.
constexpr std::uint8_t JobWAR = 1;
constexpr std::uint8_t JobMON = 23;

// NormalizeMonJob mirrors mjob/sjob == JOB_MON → both WAR.
constexpr auto NormalizeMonJob(const std::uint8_t mjob, const std::uint8_t sjob) -> std::pair<std::uint8_t, std::uint8_t>
{
    if (mjob == JobMON || sjob == JobMON)
    {
        return { JobWAR, JobWAR };
    }
    return { mjob, sjob };
}

// RaceGroupFromCharRace maps CharRace look values to grade race rows 0..4.
// CharRace: HumeM/F=1/2, ElvaanM/F=3/4, TarutaruM/F=5/6, Mithra=7, Galka=8.
constexpr auto RaceGroupFromCharRace(const std::uint8_t charRace) -> std::uint8_t
{
    switch (charRace)
    {
        case 1: // HumeMale
        case 2: // HumeFemale
            return 0;
        case 3: // ElvaanMale
        case 4: // ElvaanFemale
            return 1;
        case 5: // TarutaruMale
        case 6: // TarutaruFemale
            return 2;
        case 7: // Mithra
            return 3;
        case 8: // Galka
            return 4;
        default:
            return 0;
    }
}

// --- Main/sub level band helpers (HP path) ---

constexpr auto MainLevelOver30(const std::uint8_t mlvl) -> std::int32_t
{
    return std::clamp(static_cast<std::int32_t>(mlvl) - 30, 0, 30);
}

constexpr auto MainLevelUpTo60(const std::uint8_t mlvl) -> std::int32_t
{
    return mlvl < 60 ? static_cast<std::int32_t>(mlvl) - 1 : 59;
}

constexpr auto MainLevelOver60To75(const std::uint8_t mlvl) -> std::int32_t
{
    return std::clamp(static_cast<std::int32_t>(mlvl) - 60, 0, 15);
}

constexpr auto MainLevelOver75(const std::uint8_t mlvl) -> std::int32_t
{
    return mlvl < 75 ? 0 : static_cast<std::int32_t>(mlvl) - 75;
}

constexpr auto MainLevelOver10(const std::uint8_t mlvl) -> std::int32_t
{
    return mlvl < 10 ? 0 : static_cast<std::int32_t>(mlvl) - 10;
}

constexpr auto MainLevelOver50AndUnder60(const std::uint8_t mlvl) -> std::int32_t
{
    return std::clamp(static_cast<std::int32_t>(mlvl) - 50, 0, 10);
}

constexpr auto MainLevelOver60(const std::uint8_t mlvl) -> std::int32_t
{
    return mlvl < 60 ? 0 : static_cast<std::int32_t>(mlvl) - 60;
}

constexpr auto SubLevelOver10(const std::uint8_t slvl) -> std::int32_t
{
    return std::clamp(static_cast<std::int32_t>(slvl) - 10, 0, 20);
}

constexpr auto SubLevelOver30(const std::uint8_t slvl) -> std::int32_t
{
    return slvl < 30 ? 0 : static_cast<std::int32_t>(slvl) - 30;
}

// ComposeHPScale mirrors race/main job HP scale sum for main-level bands.
inline auto ComposeHPScale(const float base,
                           const float to60,
                           const float over30,
                           const float over60to75,
                           const float over75,
                           const std::int32_t levelUpTo60,
                           const std::int32_t levelOver30,
                           const std::int32_t levelOver60To75,
                           const std::int32_t levelOver75) -> float
{
    return base + (to60 * static_cast<float>(levelUpTo60)) + (over30 * static_cast<float>(levelOver30)) +
           (over60to75 * static_cast<float>(levelOver60To75)) + (over75 * static_cast<float>(levelOver75));
}

// BonusHPStat mirrors (over10 + over50and60) * 2.
constexpr auto BonusHPStat(const std::int32_t mainLevelOver10, const std::int32_t mainLevelOver50andUnder60) -> std::int32_t
{
    return (mainLevelOver10 + mainLevelOver50andUnder60) * 2;
}

// ComposeSubJobHP mirrors subjob HP before /2, then halves.
// sJobStat = (base + to60*(slvl-1) + over30*subOver30 + subOver30 + subOver10) / 2
inline auto ComposeSubJobHP(const float base,
                            const float to60,
                            const float over30,
                            const std::uint8_t slvl,
                            const std::int32_t subLevelOver30,
                            const std::int32_t subLevelOver10) -> float
{
    auto sJobStat = base + (to60 * static_cast<float>(slvl - 1)) + (over30 * static_cast<float>(subLevelOver30)) +
                    static_cast<float>(subLevelOver30) + static_cast<float>(subLevelOver10);
    return sJobStat / 2.0f;
}

// FinalMaxHP truncates the sum to int16.
inline auto FinalMaxHP(const float raceStat, const float jobStat, const std::int32_t bonusStat, const float sJobStat, const std::uint16_t meritBonus) -> std::int16_t
{
    return static_cast<std::int16_t>(raceStat + jobStat + static_cast<float>(bonusStat) + sJobStat + static_cast<float>(meritBonus));
}

// ComposeMPScale mirrors base + to60*levelUpTo60 + over60*levelOver60.
inline auto ComposeMPScale(const float base, const float to60, const float over60, const std::int32_t levelUpTo60, const std::int32_t levelOver60) -> float
{
    return base + to60 * static_cast<float>(levelUpTo60) + over60 * static_cast<float>(levelOver60);
}

// ComposeSubJobMP mirrors (base + to60*(slvl-1)) / sjMpDivisor.
inline auto ComposeSubJobMP(const float base, const float to60, const std::uint8_t slvl, const float sjMpDivisor) -> float
{
    return (base + to60 * static_cast<float>(slvl - 1)) / sjMpDivisor;
}

// ShouldUseSubLevelForRaceMP mirrors main MP grade == 0 && sub MP grade != 0 && slvl > 0.
constexpr auto ShouldUseSubLevelForRaceMP(const std::uint8_t mainMPGrade, const std::uint8_t subMPGrade, const std::uint8_t slvl) -> bool
{
    return mainMPGrade == 0 && subMPGrade != 0 && slvl > 0;
}

// FinalMaxMP truncates race+job+sjob+merit.
inline auto FinalMaxMP(const float raceStat, const float jobStat, const float sJobStat, const std::uint16_t meritBonus) -> std::int16_t
{
    return static_cast<std::int16_t>(raceStat + jobStat + sJobStat + static_cast<float>(meritBonus));
}

// ComposeStatScale base + to60*levelUpTo60, then optional over60/over75 with 0.01 fudge at >=75.
inline auto ComposeStatScale(const float base,
                             const float to60,
                             const float over60,
                             const float over75,
                             const std::int32_t levelUpTo60,
                             const std::int32_t levelOver60,
                             const std::int32_t levelOver75,
                             const std::uint8_t mlvl) -> float
{
    auto stat = base + to60 * static_cast<float>(levelUpTo60);
    if (levelOver60 > 0)
    {
        stat += over60 * static_cast<float>(levelOver60);
        if (levelOver75 > 0)
        {
            stat += over75 * static_cast<float>(levelOver75) - (mlvl >= 75 ? 0.01f : 0.0f);
        }
    }
    return stat;
}

// ComposeSubJobStat mirrors (base + to60*(slvl-1)) / 2.
inline auto ComposeSubJobStat(const float base, const float to60, const std::uint8_t slvl) -> float
{
    return (base + to60 * static_cast<float>(slvl - 1)) / 2.0f;
}

// FinalBaseStat truncates race+job+sjob+merit to uint16.
inline auto FinalBaseStat(const float raceStat, const float jobStat, const float sJobStat, const std::uint16_t meritBonus) -> std::uint16_t
{
    return static_cast<std::uint16_t>(raceStat + jobStat + sJobStat + static_cast<float>(meritBonus));
}

} // namespace calculatestatshelpers
