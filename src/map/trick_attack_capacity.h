#pragma once

#include <cmath>
#include <cstdint>

// Pure getAvailableTrickAttackChar / areInLine decision halves.

namespace trickattackhelpers
{

constexpr float   WorldAngleMinDistance = 0.5f;
constexpr int16_t WorldAngleMaxDeviance = 8;

// angleDifference inject: signed rotation delta already reduced to [-128,128].
constexpr auto AreInLine(const std::int16_t angleDiffAbs) -> bool
{
    return angleDiffAbs <= WorldAngleMaxDeviance;
}

// Convenience: pass raw signed angleDifference result.
constexpr auto AreInLineFromDiff(const std::int16_t angleDiff) -> bool
{
    return AreInLine(angleDiff < 0 ? static_cast<std::int16_t>(-angleDiff) : angleDiff);
}

constexpr auto ShouldEvaluateTrickAttack(const bool hasTrickAttackEffect) -> bool
{
    return hasTrickAttackEffect;
}

constexpr auto TrickAttackCandidateDistanceOK(const float distTargetMob, const float distUserMob) -> bool
{
    return distTargetMob >= WorldAngleMinDistance && distTargetMob < distUserMob;
}

constexpr auto TrickAttackCandidateSkip(const bool isSelf, const bool isDead) -> bool
{
    return isSelf || isDead;
}

// Sort key: -1 if a before b, +1 if a after b, 0 if equal.
constexpr auto CompareTrickAttackSortKey(const float distA, const float distB, const std::uint32_t idA, const std::uint32_t idB) -> int
{
    if (distA < distB)
    {
        return -1;
    }
    if (distA > distB)
    {
        return 1;
    }
    if (idA < idB)
    {
        return -1;
    }
    if (idA > idB)
    {
        return 1;
    }
    return 0;
}

} // namespace trickattackhelpers
