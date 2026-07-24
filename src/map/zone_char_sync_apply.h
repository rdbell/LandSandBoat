#pragma once

#include <cstdint>

namespace zonecharsyncapply
{

// ShouldStopForSwapLimit mirrors SpawnPCs' client-flood protection.
constexpr auto ShouldStopForSwapLimit(const std::uint8_t swapCount, const std::uint8_t maximumSwaps) -> bool
{
    return swapCount >= maximumSwaps;
}

// CanReplaceAtCapacity mirrors SpawnPCs' strict lowest-score replacement
// gate. An empty ordinary-character pool cannot make room for a candidate.
constexpr auto CanReplaceAtCapacity(const bool hasReplaceableEntry, const float candidateScore, const float lowestReplaceableScore) -> bool
{
    return hasReplaceableEntry && candidateScore > lowestReplaceableScore;
}

} // namespace zonecharsyncapply
