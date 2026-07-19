#pragma once

namespace trustcontrollernoncombatgambitadmission
{
// CanRun reports whether a non-combat trust may run gambits and notify combat listeners.
template <typename IsFollowingPath>
constexpr auto CanRun(const bool masterEngaged, IsFollowingPath&& isFollowingPath) -> bool
{
    return masterEngaged && !isFollowingPath();
}
} // namespace trustcontrollernoncombatgambitadmission
