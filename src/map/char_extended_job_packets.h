#pragma once

#include "entities/battle_entity.h"

#include <array>

namespace extendedjobpackethelpers
{

enum class Action : uint8
{
    Monstrosity,
    PUPMain,
    BLUMain,
    PUPSub,
    BLUSub,
};

struct Plan
{
    std::array<Action, 2> actions{};
    uint8                 count{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto BuildPlan(const bool hasMonstrosity, const JOBTYPE mainJob, const JOBTYPE subJob) -> Plan
{
    if (hasMonstrosity)
    {
        return { .actions = { Action::Monstrosity }, .count = 1 };
    }

    auto plan = Plan{};
    switch (mainJob)
    {
        case JOB_PUP:
            plan.actions[plan.count++] = Action::PUPMain;
            break;
        case JOB_BLU:
            plan.actions[plan.count++] = Action::BLUMain;
            break;
        default:
            break;
    }
    switch (subJob)
    {
        case JOB_PUP:
            plan.actions[plan.count++] = Action::PUPSub;
            break;
        case JOB_BLU:
            plan.actions[plan.count++] = Action::BLUSub;
            break;
        default:
            break;
    }
    return plan;
}

} // namespace extendedjobpackethelpers
