#pragma once

namespace mobbehaviorhelpers
{

inline auto CanDeaggro(const bool notorious, const bool battlefield) -> bool
{
    return !notorious && !battlefield;
}

inline auto CanBeNeutral(const bool notorious) -> bool
{
    return !notorious;
}

} // namespace mobbehaviorhelpers
