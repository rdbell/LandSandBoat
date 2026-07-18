#pragma once

namespace mobcontrollerdeaggro
{
constexpr auto CanPursueByScent(
    const bool hasScentDetection,
    const bool isInWater,
    const bool hasTarget,
    const bool targetHasDeodorize,
    const bool scentDisabled) -> bool
{
    return hasScentDetection && !isInWater && hasTarget && !targetHasDeodorize && !scentDisabled;
}

// Keep the entity-independent de-aggregation gates testable. Entity type,
// status-effect, and detection queries remain in CMobController.
constexpr auto ShouldDeaggroForHide(
    const bool targetIsThief,
    const bool targetHasHide,
    const bool canPursue,
    const bool hasTrueDetection,
    const bool hasHearingDetection) -> bool
{
    return targetIsThief && targetHasHide && !canPursue && !hasTrueDetection && !hasHearingDetection;
}

constexpr auto ShouldDeaggroForLock(
    const bool targetIsPlayer,
    const bool targetIsPet,
    const bool playerLocked,
    const bool petMasterIsPlayer,
    const bool petMasterLocked,
    const bool canPursue) -> bool
{
    const bool lockedPlayer = targetIsPlayer && playerLocked;
    const bool lockedPetOwner = targetIsPet && petMasterIsPlayer && petMasterLocked;
    return (lockedPlayer || lockedPetOwner) && !canPursue;
}
} // namespace mobcontrollerdeaggro
