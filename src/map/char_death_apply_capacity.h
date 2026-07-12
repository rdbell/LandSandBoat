#pragma once

#include "char_death_plan_capacity.h"

#include <functional>

namespace chardeathapplyhelpers
{

template <typename PushMessage,
          typename RelinquishClaim,
          typename DespawnPet,
          typename ApplyTimedDeath,
          typename SetDeathTime,
          typename ClearBlockingAid,
          typename LoseInfluence,
          typename LoseExperience,
          typename OnPlayerDeath>
inline void Apply(const chardeathplanhelpers::Plan& plan,
                  PushMessage&&      pushMessage,
                  RelinquishClaim&&  relinquishClaim,
                  DespawnPet&&       despawnPet,
                  ApplyTimedDeath&&  applyTimedDeath,
                  SetDeathTime&&     setDeathTime,
                  ClearBlockingAid&& clearBlockingAid,
                  LoseInfluence&&    loseInfluence,
                  LoseExperience&&   loseExperience,
                  OnPlayerDeath&&    onPlayerDeath)
{
    std::invoke(pushMessage, plan.message);
    std::invoke(relinquishClaim);
    if (plan.despawnPet)
    {
        std::invoke(despawnPet);
    }
    std::invoke(applyTimedDeath);
    std::invoke(setDeathTime);
    std::invoke(clearBlockingAid);
    std::invoke(loseInfluence);
    if (plan.loseEXP)
    {
        std::invoke(loseExperience, plan.retainPercent);
    }
    std::invoke(onPlayerDeath);
}

} // namespace chardeathapplyhelpers
