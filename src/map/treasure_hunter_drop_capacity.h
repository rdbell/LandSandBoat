#pragma once

#include <algorithm>
#include <cstdint>

// Pure xi.combat.treasureHunter.getDropRate.
// Parity: internal/treasurehunter (slice 0834); production wire slice 1585.

namespace treasurehunterhelpers
{

constexpr int MaxTier     = 14;
constexpr int MaxDropRate = 10000;
constexpr int BracketCount = 7;

// dropBracketTable thresholds (first match where rate >= threshold).
inline constexpr std::uint16_t DropBracketThresholds[BracketCount] = {
    2400, 1500, 1000, 500, 100, 50, 0
};

// treasureHunterTable[tier 0..14][bracket 0..6]
inline constexpr std::uint16_t TreasureHunterTable[MaxTier + 1][BracketCount] = {
    { 2400, 1500, 1000, 500, 100, 50, 10 },
    { 4800, 3000, 1200, 600, 150, 75, 20 },
    { 5600, 4000, 1500, 700, 200, 100, 30 },
    { 6000, 4250, 1650, 750, 225, 120, 35 },
    { 6400, 4500, 1800, 800, 250, 140, 40 },
    { 6666, 4750, 1900, 850, 300, 160, 45 },
    { 6800, 5000, 2000, 900, 350, 180, 50 },
    { 6900, 5250, 2100, 950, 400, 200, 60 },
    { 7050, 5500, 2250, 1050, 475, 230, 70 },
    { 7200, 5750, 2400, 1150, 550, 260, 80 },
    { 7350, 6000, 2650, 1250, 650, 300, 90 },
    { 7400, 6250, 2800, 1350, 750, 350, 100 },
    { 7600, 6500, 2950, 1550, 825, 400, 115 },
    { 7800, 6750, 3100, 1750, 900, 450, 130 },
    { 8000, 7000, 3250, 2000, 1000, 500, 150 },
};

constexpr auto ClampTier(const int thLevel) -> int
{
    return std::max(0, std::min(MaxTier, thLevel));
}

constexpr auto ClampDropRate(const int dropRate) -> int
{
    return std::max(0, std::min(MaxDropRate, dropRate));
}

// Returns 1..7 bracket (0 if none — should not occur for rate > 0).
inline auto DropBracket(const int dropRate) -> int
{
    const int rate = ClampDropRate(dropRate);
    for (int i = 0; i < BracketCount; ++i)
    {
        if (rate >= static_cast<int>(DropBracketThresholds[i]))
        {
            return i + 1;
        }
    }
    return 0;
}

// getDropRate pure.
inline auto GetDropRate(const int thLevel, const int dropRate) -> std::uint16_t
{
    const int tier = ClampTier(thLevel);
    const int rate = ClampDropRate(dropRate);
    if (rate == MaxDropRate)
    {
        return static_cast<std::uint16_t>(MaxDropRate);
    }
    if (rate == 0)
    {
        return 0;
    }
    const int bracket = DropBracket(rate);
    if (bracket < 1 || bracket > BracketCount)
    {
        return 0;
    }
    return TreasureHunterTable[tier][bracket - 1];
}

} // namespace treasurehunterhelpers
