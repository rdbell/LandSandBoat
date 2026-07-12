#pragma once

#include "char_raise_plan_capacity.h"

#include <cmath>
#include <cstdint>
#include <functional>

namespace charraiseapplyhelpers
{

template <typename SetWeaknessLevel,
          typename AddWeakness,
          typename PrepareAction,
          typename AddHP,
          typename MarkHPUpdate,
          typename PushAction,
          typename GetEXPLost,
          typename AddExperience,
          typename ClearEXPLost,
          typename AddReraise3,
          typename ClearMijin,
          typename ClearArise,
          typename ClearRaise>
inline void Apply(const charraiseplanhelpers::Plan& plan,
                  SetWeaknessLevel&& setWeaknessLevel,
                  AddWeakness&&      addWeakness,
                  PrepareAction&&    prepareAction,
                  AddHP&&            addHP,
                  MarkHPUpdate&&     markHPUpdate,
                  PushAction&&       pushAction,
                  GetEXPLost&&       getEXPLost,
                  AddExperience&&    addExperience,
                  ClearEXPLost&&     clearEXPLost,
                  AddReraise3&&      addReraise3,
                  ClearMijin&&       clearMijin,
                  ClearArise&&       clearArise,
                  ClearRaise&&       clearRaise)
{
    if (!plan.handled)
    {
        return;
    }

    std::invoke(setWeaknessLevel, plan.weaknessLevel);
    if (plan.applyWeakness)
    {
        std::invoke(addWeakness, plan.weaknessLevel, plan.weaknessDuration);
    }
    std::invoke(prepareAction, plan.animation);
    std::invoke(addHP, plan.hpReturned);
    std::invoke(markHPUpdate);
    std::invoke(pushAction);

    const auto expLost = std::invoke(getEXPLost);
    if (expLost != 0)
    {
        const auto returned = static_cast<std::uint16_t>(std::ceil(expLost * plan.expReturnRatio));
        std::invoke(addExperience, returned);
        std::invoke(clearEXPLost);
    }
    if (plan.applyReraise3)
    {
        std::invoke(addReraise3);
    }
    std::invoke(clearMijin);
    std::invoke(clearArise);
    std::invoke(clearRaise);
}

} // namespace charraiseapplyhelpers
