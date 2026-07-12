#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure capacity-point distribution math from charutils::DistributeCapacityPoints
// and eligibility gates before AddCapacityBonus/AddCapacityPoints.

namespace capacitydistributehelpers
{

// Minimum main job level and mob level for capacity awards.
constexpr std::uint8_t MinJobLevel = 99;
constexpr std::uint8_t MinMobLevelForCapacity = 100; // mobLevel > 99

// Chain timer extension after activity (seconds).
constexpr std::int64_t ChainTimerSeconds = 30;

// Chain modifier: min(1 + 0.05 * chainNumber, 1.5).
constexpr float ChainModifierCap = 1.5f;
constexpr float ChainModifierStep = 0.05f;

// ShouldAwardMember mirrors the per-alliance-member gates:
// non-null, alive, same zone, Job Breaker KI, level >= 99.
constexpr auto ShouldAwardMember(const bool isChar,
                                 const bool isDead,
                                 const bool sameZone,
                                 const bool hasJobBreaker,
                                 const std::uint8_t mainLevel) -> bool
{
    if (!isChar || isDead || !sameZone)
    {
        return false;
    }
    if (!hasJobBreaker || mainLevel < MinJobLevel)
    {
        return false;
    }
    return true;
}

// ShouldComputeCapacity mirrors mobLevel > 99.
constexpr auto ShouldComputeCapacity(const std::uint8_t mobLevel) -> bool
{
    return mobLevel > MinJobLevel;
}

// LevelDiff is mobLevel - 99 (caller ensures mobLevel >= 100 for awards).
constexpr auto LevelDiff(const std::uint8_t mobLevel) -> std::int16_t
{
    return static_cast<std::int16_t>(mobLevel) - static_cast<std::int16_t>(MinJobLevel);
}

// BaseCapacityPoints mirrors the cubic base formula for levelDiff = mobLevel-99:
//   0.0089*d^3 + 0.0533*d^2 + 3.7439*d + 89.7
inline auto BaseCapacityPoints(const std::int16_t levelDiff) -> float
{
    const auto d = static_cast<float>(levelDiff);
    return 0.0089f * d * d * d + 0.0533f * d * d + 3.7439f * d + 89.7f;
}

// ChainModifier mirrors min(1 + 0.05 * chainNumber, 1.5).
inline auto ChainModifier(const std::uint16_t chainNumber) -> float
{
    return std::min(1.0f + ChainModifierStep * static_cast<float>(chainNumber), ChainModifierCap);
}

// ApplyChainModifier multiplies base capacity by the chain modifier.
inline auto ApplyChainModifier(const float capacityPoints, const std::uint16_t chainNumber) -> float
{
    return capacityPoints * ChainModifier(chainNumber);
}

// IsChainActive mirrors chainTime > now || chainTime == min().
template <typename TimePoint>
constexpr auto IsChainActive(const TimePoint chainTime, const TimePoint now, const TimePoint minTime) -> bool
{
    return chainTime > now || chainTime == minTime;
}

// ShouldExtendChainTimer mirrors chainActive path that refreshes timer after award path setup.
// When chain was inactive, number is reset to 1 and timer set; when active, timer is refreshed.
// Host applies timer writes; this only classifies the inactive-reset case.
constexpr auto ShouldResetInactiveChain(const bool chainActive) -> bool
{
    return !chainActive;
}

// NextChainNumberAfterAward is chainNumber+1 after a capacity-chain award message.
constexpr auto NextChainNumberAfterAward(const std::uint16_t chainNumber) -> std::uint16_t
{
    return static_cast<std::uint16_t>(chainNumber + 1);
}

// ShouldSendChainMessage mirrors levelDiff >= 1 && isCapacityChain.
constexpr auto ShouldSendChainMessage(const std::int16_t levelDiff, const bool isCapacityChain) -> bool
{
    return levelDiff >= 1 && isCapacityChain;
}

// HasNonZeroChainNumber mirrors chainNumber != 0 for CapacityChain vs CapacityPointsGained msg.
constexpr auto HasNonZeroChainNumber(const std::uint16_t chainNumber) -> bool
{
    return chainNumber != 0;
}

} // namespace capacitydistributehelpers
