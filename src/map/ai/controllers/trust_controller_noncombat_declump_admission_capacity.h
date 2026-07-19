#pragma once

namespace trustcontrollernoncombatdeclumpadmission
{
// ShouldDeclump reports whether a nearby idle party trust should be separated.
template <typename IsTooClose, typename IsFollowingPath>
constexpr auto ShouldDeclump(const bool isSelf, IsTooClose&& isTooClose, IsFollowingPath&& isFollowingPath) -> bool
{
    return !isSelf && isTooClose() && !isFollowingPath();
}
} // namespace trustcontrollernoncombatdeclumpadmission
