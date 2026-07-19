#pragma once

namespace mobcontrollerengagedtargetrange
{
// IsInRange reports whether the current battle target is within melee range.
// The range callback is evaluated only for the current battle target.
template <typename GetMeleeRange>
constexpr auto IsInRange(const bool isEngagedTarget, const float distance, GetMeleeRange&& getMeleeRange) -> bool
{
    return isEngagedTarget && distance <= getMeleeRange();
}
} // namespace mobcontrollerengagedtargetrange
