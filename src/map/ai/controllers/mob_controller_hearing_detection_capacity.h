#pragma once

namespace mobcontrollerhearingdetection
{
// CanDetect reports whether hearing detection passes its gates and visibility route.
template <typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectHearing,
    const float distance,
    const float soundRange,
    const bool hasSneak,
    const bool targetInMeleeRange,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!(detectHearing && distance < soundRange && !hasSneak))
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrollerhearingdetection
