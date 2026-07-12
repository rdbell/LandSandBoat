#pragma once

#include "enums/action/animation.h"

#include <algorithm>
#include <chrono>
#include <cstdint>

namespace charraiseplanhelpers
{

struct Input
{
    std::uint8_t  hasRaise{};
    std::uint8_t  weaknessLevel{};
    bool          hasArise{};
    bool          mijinGakure{};
    bool          mijinReraise{};
    std::int32_t  maxHP{};
    std::uint8_t  mainLevel{};
    std::uint8_t  expRetain{};
};

struct Plan
{
    bool                      handled{};
    std::uint8_t              weaknessLevel{};
    bool                      applyWeakness{};
    std::chrono::minutes      weaknessDuration{};
    ActionAnimation           animation{};
    std::uint16_t             hpReturned{};
    double                    expReturnRatio{};
    bool                      applyReraise3{};
};

inline auto Build(const Input& input) -> Plan
{
    if (input.hasRaise == 0)
    {
        return {};
    }

    Plan plan{
        .handled          = true,
        .weaknessLevel    = input.weaknessLevel == 0 ? std::uint8_t{ 1 } : input.weaknessLevel,
        .applyWeakness    = !input.mijinGakure,
        .hpReturned       = 1,
        .applyReraise3    = input.hasArise,
    };
    if (plan.applyWeakness)
    {
        plan.weaknessDuration = input.hasArise ? std::chrono::minutes{ 3 } : std::chrono::minutes{ 5 };
    }
    const auto retainMultiplier = static_cast<double>(1 - input.expRetain);

    if (input.mijinGakure && input.mijinReraise)
    {
        plan.animation  = ActionAnimation::Raise;
        plan.hpReturned = static_cast<std::uint16_t>(input.maxHP);
    }
    else if (input.hasRaise == 1)
    {
        plan.animation      = ActionAnimation::Raise;
        plan.hpReturned     = static_cast<std::uint16_t>(input.mijinGakure ? input.maxHP * 0.5 : input.maxHP * 0.1);
        plan.expReturnRatio = 0.50 * retainMultiplier;
    }
    else if (input.hasRaise == 2)
    {
        plan.animation      = ActionAnimation::Raise2;
        plan.hpReturned     = static_cast<std::uint16_t>(input.mijinGakure ? input.maxHP * 0.5 : input.maxHP * 0.25);
        plan.expReturnRatio = (input.mainLevel <= 50 ? 0.50 : 0.75) * retainMultiplier;
    }
    else if (input.hasRaise == 3)
    {
        plan.animation      = ActionAnimation::Raise3;
        plan.hpReturned     = static_cast<std::uint16_t>(input.maxHP * 0.5);
        plan.expReturnRatio = (input.mainLevel <= 50 ? 0.50 : 0.90) * retainMultiplier;
    }
    else if (input.hasRaise == 4)
    {
        plan.animation      = ActionAnimation::Arise;
        plan.hpReturned     = static_cast<std::uint16_t>(input.maxHP);
        plan.expReturnRatio = (input.mainLevel <= 50 ? 0.50 : 0.90) * retainMultiplier;
    }

    plan.hpReturned = std::max<std::uint16_t>(1, plan.hpReturned);
    return plan;
}

} // namespace charraiseplanhelpers
