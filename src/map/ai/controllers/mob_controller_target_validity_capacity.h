#pragma once

namespace mobcontrollertargetvalidity
{
constexpr auto ShouldDeaggroNoTarget(const bool hasTarget, const bool enmityEmpty) -> bool
{
    return !hasTarget && enmityEmpty;
}

constexpr auto TargetInvalid(
    const bool hasTarget, const bool targetAlive, const bool targetMounted, const bool sameZone,
    const bool sameConfrontation, const bool differentAllegiance, const bool detectionAllows,
    const bool hideAllows, const bool lockAllows, const bool sameBattle) -> bool
{
    return !hasTarget || !targetAlive || targetMounted || !sameZone || !sameConfrontation || !differentAllegiance ||
           !detectionAllows || !hideAllows || !lockAllows || !sameBattle;
}

// TargetInvalidWithChecks evaluates detection, Hide, and Lock only after the
// target passes the preceding static validity gates.
template <typename DetectionAllows, typename HideAllows, typename LockAllows>
constexpr auto TargetInvalidWithChecks(
    const bool targetAlive, const bool targetMounted, const bool sameZone,
    const bool sameConfrontation, const bool differentAllegiance,
    DetectionAllows&& detectionAllows, HideAllows&& hideAllows, LockAllows&& lockAllows,
    const bool sameBattle) -> bool
{
    if (!targetAlive || targetMounted || !sameZone || !sameConfrontation || !differentAllegiance)
    {
        return true;
    }
    return !detectionAllows() || !hideAllows() || !lockAllows() || !sameBattle;
}
} // namespace mobcontrollertargetvalidity
