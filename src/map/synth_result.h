#pragma once

#include "common/cbasetypes.h"

namespace synthresulthelpers
{

struct SkillPlan
{
    float successRate{};
    uint8 hqTier{};
    bool  canHQ{};

    constexpr auto operator==(const SkillPlan&) const -> bool = default;
};

constexpr auto MakeSkillPlan(const int16 difficulty, const int16 successRateMod, const bool skillAllowsHQ) -> SkillPlan
{
    auto plan = SkillPlan{
        .successRate = 95.0f,
        .hqTier      = 0,
        .canHQ       = skillAllowsHQ,
    };

    if (difficulty >= 4)
    {
        plan.successRate = 80.0f - 10.0f * (difficulty - 3);
        plan.canHQ       = false;
    }
    else if (difficulty >= 1)
    {
        plan.successRate = 95.0f - 5.0f * difficulty;
        plan.canHQ       = false;
    }
    else if (difficulty >= -10)
    {
        plan.hqTier = 1;
    }
    else if (difficulty >= -30)
    {
        plan.hqTier = 2;
    }
    else if (difficulty >= -50)
    {
        plan.hqTier = 3;
    }
    else
    {
        plan.hqTier = 4;
    }

    plan.successRate += successRateMod;
    if (!skillAllowsHQ)
    {
        plan.successRate += 1.0f;
    }
    if (plan.successRate > 99.0f)
    {
        plan.successRate = 99.0f;
    }
    return plan;
}

} // namespace synthresulthelpers
