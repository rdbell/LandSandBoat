#pragma once

#include <cstddef>

namespace zonecharsyncexisting
{

// TotalScore mirrors SpawnPCs' score for an already spawned character.
constexpr auto TotalScore(const float significance, const float bonus, const float distance, const float swapThreshold) -> float
{
    return significance + bonus - distance + swapThreshold;
}

// ShouldTrackForSwap excludes visible-GM, party, and alliance entries from
// the ordinary-character pool that may be replaced to make room.
constexpr auto ShouldTrackForSwap(const float significance, const float allianceSignificance) -> bool
{
    return significance < allianceSignificance;
}

// ShouldAddToSwapPool mirrors the bounded min-heap admission: below capacity
// every entry is retained; at capacity only a strictly better score replaces
// the current lowest entry.
constexpr auto ShouldAddToSwapPool(const std::size_t currentCount,
                                   const std::size_t maximumCount,
                                   const float       lowestScore,
                                   const float       candidateScore) -> bool
{
    return currentCount < maximumCount || (currentCount >= maximumCount && lowestScore < candidateScore);
}

} // namespace zonecharsyncexisting
