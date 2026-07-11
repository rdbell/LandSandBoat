#pragma once

namespace petvalidtargethelpers
{

template <typename BaseValidTarget>
inline auto Apply(
    const bool playerTarget,
    const bool sameAllegiance,
    BaseValidTarget&& baseValidTarget) -> bool
{
    if (playerTarget && sameAllegiance)
    {
        return false;
    }
    return baseValidTarget();
}

} // namespace petvalidtargethelpers
