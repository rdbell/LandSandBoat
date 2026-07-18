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
} // namespace mobcontrollertargetvalidity
