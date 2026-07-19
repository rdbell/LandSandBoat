#pragma once

#include <cstdint>

namespace mobcontrollerlowhpdetection
{
// CanDetect reports whether low-HP detection passes its gates and visibility route.
template <typename CanSeeTarget>
constexpr auto CanDetect(
    const bool detectLowHP,
    const std::uint8_t hpPercent,
    const bool targetInMeleeRange,
    CanSeeTarget&& canSeeTarget) -> bool
{
    if (!(detectLowHP && hpPercent < 75))
    {
        return false;
    }
    return targetInMeleeRange || canSeeTarget();
}
} // namespace mobcontrollerlowhpdetection
