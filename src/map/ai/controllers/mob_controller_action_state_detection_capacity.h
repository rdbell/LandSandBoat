#pragma once

namespace mobcontrolleractionstatedetection
{
// CanDetect reports whether action-state detection passes its gates and visibility route.
template <typename TargetUsingAction, typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectAction,
    const bool targetInMeleeRange,
    TargetUsingAction&& targetUsingAction,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!detectAction || !targetUsingAction())
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrolleractionstatedetection
