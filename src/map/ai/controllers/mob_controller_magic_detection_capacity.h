#pragma once

namespace mobcontrollermagicdetection
{
// CanDetect reports whether magic detection passes its gates and visibility route.
template <typename TargetCastingMPCostSpell, typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectMagic,
    const float distance,
    const float magicRange,
    const bool targetInMeleeRange,
    TargetCastingMPCostSpell&& targetCastingMPCostSpell,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!(detectMagic && distance < magicRange))
    {
        return false;
    }
    if (!targetCastingMPCostSpell())
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrollermagicdetection
