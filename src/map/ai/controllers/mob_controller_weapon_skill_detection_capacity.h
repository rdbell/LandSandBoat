#pragma once

namespace mobcontrollerweaponskilldetection
{
// CanDetect reports whether weapon-skill detection passes its gates and visibility route.
template <typename TargetUsingWeaponSkill, typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectWeaponSkill,
    const bool targetInMeleeRange,
    TargetUsingWeaponSkill&& targetUsingWeaponSkill,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!detectWeaponSkill || !targetUsingWeaponSkill())
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrollerweaponskilldetection
