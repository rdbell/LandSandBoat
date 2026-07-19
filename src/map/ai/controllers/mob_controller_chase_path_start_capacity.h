#pragma once

namespace mobcontrollerchasepathstart
{
// ShouldStart reports whether Move should begin a new chase path.
constexpr auto ShouldStart(const bool followingPath, const bool outOfAttackRange) -> bool
{
    return !followingPath && outOfAttackRange;
}
} // namespace mobcontrollerchasepathstart
