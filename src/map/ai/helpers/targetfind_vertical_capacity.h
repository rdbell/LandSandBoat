#pragma once

namespace targetfindverticalhelpers
{

// ExceedsVerticalRange mirrors validEntity's inclusive vertical cap. Mob
// self-centered AoEs receive 8.5y; every other target search receives 8y.
inline auto ExceedsVerticalRange(const float yDelta, const bool selfCenteredAoE, const bool casterIsMob) -> bool
{
    const float cap = selfCenteredAoE && casterIsMob ? 8.5f : 8.0f;
    return yDelta >= cap;
}

} // namespace targetfindverticalhelpers
