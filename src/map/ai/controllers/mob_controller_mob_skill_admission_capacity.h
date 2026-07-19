#pragma once

namespace mobcontrollermobskilladmission
{
// CanDispatch reports whether a resolved mob skill may start.
constexpr auto CanDispatch(const bool hasTarget, const bool isAstralFlow, const bool scriptAccepted, const bool targetInRange) -> bool
{
    return hasTarget && !isAstralFlow && scriptAccepted && targetInRange;
}
} // namespace mobcontrollermobskilladmission
