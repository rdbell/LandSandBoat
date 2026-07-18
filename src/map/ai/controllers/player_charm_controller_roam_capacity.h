#pragma once

#include <cstdint>

namespace playercharmcontrollerroam
{

enum class Action : uint8_t
{
    Hold,
    Path,
    Warp,
};

struct Plan
{
    bool   engageMasterTarget;
    Action action;
};

inline auto Resolve(bool masterEngaged, bool pathFinderExists, bool hasSpeed, float distanceToMaster) -> Plan
{
    Plan plan{ masterEngaged, Action::Hold };
    if (!pathFinderExists || distanceToMaster <= 2.1f)
    {
        return plan;
    }
    if (distanceToMaster < 35.0f)
    {
        plan.action = Action::Path;
    }
    else if (hasSpeed)
    {
        plan.action = Action::Warp;
    }
    return plan;
}

} // namespace playercharmcontrollerroam
