#pragma once

namespace mobcontrollerdetectiontarget
{
// CanDetect reports whether a target is eligible for detection checks.
template <typename TargetDead, typename TargetMounted>
constexpr auto CanDetect(const bool hasTarget, TargetDead&& targetDead, TargetMounted&& targetMounted) -> bool
{
    if (!hasTarget || targetDead())
    {
        return false;
    }
    return !targetMounted();
}
} // namespace mobcontrollerdetectiontarget
