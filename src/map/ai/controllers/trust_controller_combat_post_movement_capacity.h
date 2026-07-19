#pragma once

namespace trustcontrollercombatpostmovement
{
struct Plan
{
    bool declump;
    bool followPath;
};

constexpr auto ShouldDeclump(const bool followingPath) -> bool
{
    return !followingPath;
}

constexpr auto ShouldFollowPath(const bool inTransit) -> bool
{
    return !inTransit;
}

constexpr auto Resolve(const bool followingPath, const bool inTransit) -> Plan
{
    return { ShouldDeclump(followingPath), ShouldFollowPath(inTransit) };
}
} // namespace trustcontrollercombatpostmovement
