#pragma once

#include "common/cbasetypes.h"

// Pure Abyssea Dominion helpers shared by dual-wire slices:
//   - 2873: XPMultiplier / RewardExp residual dual-wire suite
//           (completeDominionOp level scale)
//   - 3156: XPMultiplier / RewardExp dedicated dual-wire (xp_multiplier.go)
//
// Dual-wire index:
//   - 2873: XPMultiplier / RewardExp residual dual-wire suite
//   - 3156: XPMultiplier / RewardExp dedicated dual-wire
//           (if mainLvl < 75 then 1 - (75 - mainLvl) * 0.02 else 1.0)
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
// Residual dual-wire suite: slice 2873 / test_dominion_xp_multiplier_2873.
// Dedicated dual-wire suite: slice 3156 / test_dominion_xp_multiplier_3156.
// Dual-wire of Go dominion.XPMultiplier / RewardExp.

namespace dominionhelpers
{

// XPMultiplier is the pure free-function form of the completeDominionOp level
// scale:
//   if mainLvl < 75 then 1 - (75 - mainLvl) * 0.02 else 1.0
// Level 75+ → 1.0; level 25 → 0.0; below 25 → negative (mirrors Lua).
// Future Lua host injects mainLvl into this helper instead of re-inlining
// the arithmetic.
// Formula unchanged from pure 1033 / residual 2873; dedicated dual-wire
// expansion is slice 3156.
inline auto XPMultiplier(const int32 mainLvl) -> double
{
    if (mainLvl < 75)
    {
        return 1.0 - static_cast<double>(75 - mainLvl) * 0.02;
    }
    return 1.0;
}

// RewardExp is dominionOpQuests[opID][3] * XPMultiplier (pre-addExp inject).
// Dual-wire of Go dominion.RewardExp (slice 3156 dedicated / 2873 residual).
inline auto RewardExp(const uint16 baseReward, const int32 mainLvl) -> double
{
    return static_cast<double>(baseReward) * XPMultiplier(mainLvl);
}

} // namespace dominionhelpers
