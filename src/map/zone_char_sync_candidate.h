#pragma once

#include <cstddef>

namespace zonecharsynccandidate
{

// HasEligibleIdentity mirrors considerCandidate's hidden-GM and Mog House
// rejection after the caller has handled null, self, and already-spawned
// candidates.
constexpr auto HasEligibleIdentity(const bool targetGMHidden, const bool sameMogHouse) -> bool
{
    return !targetGMHidden && sameMogHouse;
}

// IsInCandidateRange combines SpawnPCs' horizontal and vertical gates.
constexpr auto IsInCandidateRange(const bool withinDistance, const bool withinVerticalDistance) -> bool
{
    return withinDistance && withinVerticalDistance;
}

// ShouldAdmit mirrors considerCandidate's pre-heap admission. A candidate is
// always eligible below the visible-character cap; at capacity it must beat
// the lowest replaceable existing character.
constexpr auto ShouldAdmit(const std::size_t spawnedCount,
                           const std::size_t maximumCount,
                           const bool        swapPoolEmpty,
                           const float       lowestSwapScore,
                           const float       candidateScore) -> bool
{
    return spawnedCount < maximumCount || (!swapPoolEmpty && candidateScore > lowestSwapScore);
}

} // namespace zonecharsynccandidate
