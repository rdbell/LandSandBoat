#pragma once

namespace trustcontrollercombatdeclumpadmission
{
// ShouldDeclump reports whether a nearby idle party trust should be separated in combat.
template <typename IsFollowingPath, typename IsClose>
constexpr auto ShouldDeclump(const bool isSelf, IsFollowingPath&& isFollowingPath, IsClose&& isClose) -> bool
{
    return !isSelf && !isFollowingPath() && isClose();
}
} // namespace trustcontrollercombatdeclumpadmission
