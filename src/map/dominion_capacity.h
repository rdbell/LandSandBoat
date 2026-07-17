#pragma once

#include "common/cbasetypes.h"

// Pure Abyssea Dominion helpers shared by dual-wire slices:
//   - 2873: XPMultiplier / RewardExp (completeDominionOp level scale)
//
// Lua production host: scripts/globals/abyssea/dominion.lua
//   local function completeDominionOp(player, opID):
//
//   local xpMultiplier = 1
//   if player:getMainLvl() < 75 then
//     xpMultiplier = xpMultiplier - (75 - player:getMainLvl()) * .02
//   end
//   player:addExp(dominionOpQuests[opID][3] * xpMultiplier)
//
// Host injects scalars only (no player / entity pointers):
//   mainLvl    — player:getMainLvl()
//   baseReward — dominionOpQuests[opID][3] (baseRewardValue = 1000)
//
// Entity completeQuest / addExp / addCurrency / clearOpVars remains host-owned.
// Prior pure port: OmegaXI slice 1033 (internal/dominion).

namespace dominionhelpers
{

// XPMultiplier is the pure free-function form of the completeDominionOp level
// scale:
//   if mainLvl < 75 then 1 - (75 - mainLvl) * 0.02 else 1.0
// Level 75+ → 1.0; level 25 → 0.0; below 25 → negative (mirrors Lua).
// Future Lua host injects mainLvl into this helper instead of re-inlining
// the arithmetic.
inline auto XPMultiplier(const int32 mainLvl) -> double
{
    if (mainLvl < 75)
    {
        return 1.0 - static_cast<double>(75 - mainLvl) * 0.02;
    }
    return 1.0;
}

// RewardExp is dominionOpQuests[opID][3] * XPMultiplier (pre-addExp inject).
inline auto RewardExp(const uint16 baseReward, const int32 mainLvl) -> double
{
    return static_cast<double>(baseReward) * XPMultiplier(mainLvl);
}

} // namespace dominionhelpers
