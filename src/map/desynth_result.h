#pragma once

#include "common/cbasetypes.h"

namespace desynthresulthelpers
{

struct SkillPlan
{
    float successRate{};
    bool  canHQ{};

    constexpr auto operator==(const SkillPlan&) const -> bool = default;
};

constexpr auto MakeSkillPlan(const int16 difficulty, const int16 successRateMod, const bool skillAllowsHQ) -> SkillPlan
{
    auto plan = SkillPlan{
        .successRate = 40.0f,
        .canHQ       = skillAllowsHQ,
    };

    if (difficulty >= 8)
    {
        plan.successRate = 10.0f - 10.0f * (difficulty - 7) / 3.0f;
    }
    else if (difficulty >= 1)
    {
        plan.successRate = 40.0f - 5.0f * (difficulty - 1);
    }

    plan.successRate += successRateMod;
    if (!skillAllowsHQ)
    {
        plan.successRate += 1.0f;
    }
    return plan;
}

} // namespace desynthresulthelpers
