#pragma once

namespace playercharmcontrollercombat
{

struct Plan
{
    bool disengage;
    bool syncTarget;
    bool lookAtTarget;
    bool pursueTarget;
};

inline auto ShouldDisengage(bool masterEngaged) -> bool
{
    return !masterEngaged;
}

inline auto ShouldSyncTarget(bool targetMismatch) -> bool
{
    return targetMismatch;
}

inline auto Resolve(bool masterEngaged, bool targetMismatch, bool hasTarget, bool canFollowPath,
                    bool canAttack, bool hasSpeed) -> Plan
{
    Plan plan{ ShouldDisengage(masterEngaged), ShouldSyncTarget(targetMismatch), false, false };
    if (hasTarget && canFollowPath)
    {
        plan.lookAtTarget = true;
        plan.pursueTarget = !canAttack && hasSpeed;
    }
    return plan;
}

} // namespace playercharmcontrollercombat
