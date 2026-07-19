#pragma once

namespace mobcontrollersightdetection
{
// CanDetect reports whether sight detection passes its gates and visibility route.
template <typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectSight,
    const bool hasInvisible,
    const float distance,
    const float sightRange,
    const bool facingTarget,
    const bool targetInMeleeRange,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!(detectSight && !hasInvisible && distance < sightRange && facingTarget))
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrollersightdetection
