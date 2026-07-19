#pragma once

#include <cstdint>

namespace trustcontrollercombatmovement
{

enum class Action : uint8_t
{
    Hold,
    PathOut,
    MeleePath,
    MeleeStep,
};

struct Plan
{
    Action action;
    float  desiredDistance;
};

constexpr auto CanRun(const bool canFollowPath, const bool hasSpeed) -> bool
{
    return canFollowPath && hasSpeed;
}

inline auto Resolve(bool canFollowPath, bool hasSpeed, int16_t movementDistance, float distanceToTarget,
                    float distanceToMaster, bool canAttack) -> Plan
{
    if (!CanRun(canFollowPath, hasSpeed))
    {
        return {};
    }

    switch (movementDistance)
    {
        case -1:
            if (distanceToMaster > 15.0f || distanceToTarget > 15.0f)
            {
                return { Action::PathOut, 9.0f };
            }
            break;
        case -2:
            break;
        case 0:
            if (!canAttack && distanceToTarget > 3.0f)
            {
                if (distanceToTarget < 9.0f)
                {
                    return { Action::MeleePath, 3.0f };
                }
                return { Action::MeleeStep, 0.0f };
            }
            break;
        default:
            return { Action::PathOut, static_cast<float>(movementDistance) };
    }
    return {};
}

} // namespace trustcontrollercombatmovement
