#pragma once

namespace mobcontrolleroverlapreposition
{
// ShouldReposition reports whether another spawned mob warrants a reposition.
constexpr auto ShouldReposition(const bool isSelf, const bool followingPath, const bool withinOverlapRange) -> bool
{
    return !isSelf && !followingPath && withinOverlapRange;
}
} // namespace mobcontrolleroverlapreposition
