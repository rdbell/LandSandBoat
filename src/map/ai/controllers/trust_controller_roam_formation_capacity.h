#pragma once

#include <cstdint>

namespace trustcontrollerroamformation
{

constexpr float PathLimit    = 9.0f;
constexpr float WarpDistance = 30.0f;

enum class Action : uint8_t
{
    None,
    Declump,
    Path,
    Step,
    Warp,
    Clear,
};

struct Plan
{
    Action action;
    float  targetDistance;
};

inline auto Resolve(uint8_t partyPosition, float currentDistance, bool followingPath) -> Plan
{
    float targetDistance  = 3.0f;
    float declumpDistance = 1.5f;
    float followMax       = 3.5f;
    if (partyPosition == 0)
    {
        targetDistance  = 1.5f;
        declumpDistance = 1.0f;
        followMax       = 2.0f;
    }

    if (currentDistance < declumpDistance)
    {
        return { Action::Declump, targetDistance };
    }
    if (currentDistance > followMax && currentDistance > WarpDistance)
    {
        return { Action::Warp, targetDistance };
    }
    if (currentDistance > followMax && currentDistance < PathLimit)
    {
        return { Action::Path, targetDistance };
    }
    if (currentDistance > followMax)
    {
        return { Action::Step, targetDistance };
    }
    if (followingPath)
    {
        return { Action::Clear, targetDistance };
    }
    return { Action::None, targetDistance };
}

} // namespace trustcontrollerroamformation
