#pragma once

#include "common/cbasetypes.h"

// Pure Synergy furnace helpers shared by dual-wire slices:
//   - 2877: CanClaimFurnace residual dual-wire suite (AVAILABLE gate)
//   - 2896: CanTradeIntoFurnace residual dual-wire suite (CLAIMED+owner)
//   - 2899: CanOperateFurnace residual dual-wire suite (claimedByYou gate)
//   - 3065: CanClaimFurnace prior dedicated dual-wire (claim_furnace.go)
//   - 3098: CanTradeIntoFurnace prior dedicated dual-wire (trade_furnace.go)
//   - 3117: CanOperateFurnace prior dedicated dual-wire (operate_furnace.go)
//   - 3239: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated 3065; formula unchanged)
//   - 3285: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3239; formula unchanged)
//   - 3315: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3285; formula unchanged)
//   - 3359: CanTradeIntoFurnace dedicated dual-wire expand residual 2896
//           (prior dedicated 3098; formula unchanged)
//   - 3383: CanOperateFurnace prior dedicated dual-wire expand residual 2899
//           (prior dedicated 3117; formula unchanged)
//   - 3439: CanOperateFurnace dedicated dual-wire expand residual 2899
//           (prior dedicated expand 3383; prior dedicated 3117; formula unchanged)
//   - 3494: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3315; formula unchanged)
//   - 3551: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3494; formula unchanged)
//   - 3596: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3551; formula unchanged)
//   - 3641: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3596; formula unchanged)
//   - 3686: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3641; formula unchanged)
//   - 3731: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3686; formula unchanged)
//   - 3776: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3731; formula unchanged)
//   - 3821: CanClaimFurnace prior dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3776; formula unchanged)
//   - 3866: CanClaimFurnace dedicated dual-wire expand residual 2877
//           (prior dedicated expand 3821; formula unchanged)
//
// Dual-wire index:
//   - 2877: CanClaimFurnace residual dual-wire suite
//   - 2896: CanTradeIntoFurnace residual dual-wire suite
//   - 2899: CanOperateFurnace residual dual-wire suite
//   - 3065: CanClaimFurnace prior dedicated (state == FurnaceAvailable)
//   - 3098: CanTradeIntoFurnace prior dedicated
//           (state == FurnaceClaimed && IsClaimedBy)
//   - 3117: CanOperateFurnace prior dedicated (IsClaimedBy alias)
//   - 3239: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated 3065)
//   - 3285: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3239)
//   - 3315: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3285)
//   - 3359: CanTradeIntoFurnace = state == FurnaceClaimed && IsClaimedBy
//           dedicated dual-wire expand residual 2896 (prior dedicated 3098)
//   - 3383: CanOperateFurnace = IsClaimedBy(furnacePlayerID, playerID)
//           prior dedicated dual-wire expand residual 2899 (prior dedicated 3117)
//   - 3439: CanOperateFurnace = IsClaimedBy(furnacePlayerID, playerID)
//           dedicated dual-wire expand residual 2899 (prior dedicated expand 3383;
//           prior dedicated 3117)
//   - 3494: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3315)
//   - 3551: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3494)
//   - 3596: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3551)
//   - 3641: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3596)
//   - 3686: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3641)
//   - 3731: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3686)
//   - 3776: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3731)
//   - 3821: CanClaimFurnace = state == FurnaceAvailable
//           prior dedicated dual-wire expand residual 2877 (prior dedicated expand 3776)
//   - 3866: CanClaimFurnace = state == FurnaceAvailable
//           dedicated dual-wire expand residual 2877 (prior dedicated expand 3821)
//
// Production hosts are Lua under scripts/globals/synergy.lua
// (furnaceStates + synergyFurnaceOnTrigger / synergyFurnaceOnTrade).
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining state == AVAILABLE / CLAIMED and
// claimedByYou. Helpers take host-injected scalars only (no player /
// npc / entity pointers). Side effects (CLAIM_SET message,
// attachToSynergyFurnace, distance checker, ENABLE_SYNERGY, recipe
// lookup, trade consume, startEvent) remain host-owned.
// Go dual-wire: synergy.CanClaimFurnace
// (internal/synergy/claim_furnace.go). Future Lua host injects
// CanClaimFurnace then CLAIM_SET + attachToSynergyFurnace.
// Go dual-wire: synergy.CanTradeIntoFurnace
// (internal/synergy/trade_furnace.go). Future Lua host injects
// CanTradeIntoFurnace then recipe lookup / fewell / startEvent 4521.
// Go dual-wire: synergy.CanOperateFurnace
// (internal/synergy/operate_furnace.go). Future Lua host injects
// CanOperateFurnace then CLAIMED menu / ACTIVE operate / COMPLETED retrieve.
//
// Prior pure port: OmegaXI slice 1149 (internal/synergy furnace.go).
// Coverage: test_synergy_claim_furnace_2877 (residual),
// test_synergy_claim_furnace_3065 (prior dedicated dual-wire; not in CMake/main),
// test_synergy_claim_furnace_3239 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3285 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3315 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3494 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3551 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3596 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3641 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3686 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3731 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3776 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3821 (prior dedicated expand residual 2877; not in CMake/main),
// test_synergy_claim_furnace_3866 (dedicated expand residual 2877; not in CMake/main),
// test_synergy_trade_furnace_2896 (residual),
// test_synergy_trade_furnace_3098 (prior dedicated dual-wire; not in CMake/main),
// test_synergy_trade_furnace_3359 (dedicated expand residual 2896; not in CMake/main),
// test_synergy_operate_furnace_2899 (residual),
// test_synergy_operate_furnace_3117 (prior dedicated dual-wire; not in CMake/main),
// test_synergy_operate_furnace_3383 (prior dedicated expand residual 2899; not in CMake/main),
// test_synergy_operate_furnace_3439 (dedicated expand residual 2899; not in CMake/main).

namespace synergyhelpers
{

// furnaceStates.AVAILABLE — npc local-var synergyFurnaceState when free.
// Matches Go synergy.FurnaceAvailable and Lua furnaceStates.AVAILABLE.
constexpr uint8 FurnaceAvailable = 0;

// furnaceStates.CLAIMED — npc local-var synergyFurnaceState when claimed.
// Matches Go synergy.FurnaceClaimed and Lua furnaceStates.CLAIMED.
constexpr uint8 FurnaceClaimed = 1;

// ---------------------------------------------------------------------------
// Slice 2877 residual / 3065 prior dedicated / 3239 prior expand residual 2877
// / 3285 prior expand residual 2877 / 3315 prior expand residual 2877
// / 3494 prior expand residual 2877 / 3551 prior expand residual 2877
// / 3596 prior expand residual 2877 / 3641 prior expand residual 2877
// / 3686 prior expand residual 2877 / 3731 prior expand residual 2877
// / 3776 prior expand residual 2877 / 3821 prior expand residual 2877
// / 3866 dedicated expand residual 2877
// — synergyFurnaceOnTrigger AVAILABLE claim gate
// ---------------------------------------------------------------------------

// CanClaimFurnace mirrors the pure AVAILABLE gate of synergyFurnaceOnTrigger
// before CLAIM_SET + attachToSynergyFurnace:
//   if furnaceState == furnaceStates.AVAILABLE then
//     player:messageSpecial(CLAIM_SET, SYNERGY_CRUCIBLE)
//     xi.synergy.attachToSynergyFurnace(player, npc)
//   end
//
// Formula (slice 3866 dual-wire expand residual 2877; prior dedicated expand
// 3821; prior dedicated expand 3776; prior dedicated expand 3731; prior
// dedicated expand 3686; prior dedicated expand 3641; prior dedicated expand
// 3596; prior dedicated expand 3551; prior dedicated expand 3494; prior
// dedicated expand 3315; prior dedicated expand 3285; prior dedicated expand
// 3239; prior dedicated 3065):
//   CanClaimFurnace(state) = state == FurnaceAvailable
//
// state is the host-injected npc:getLocalVar(synergyFurnaceState).
// true  → host CLAIM_SET + attachToSynergyFurnace
// false → other handleFurnaceState branches (CLAIMED / ACTIVE / COMPLETED)
//
// Dual-wire of Go synergy.CanClaimFurnace.
// Call site: future Lua synergyFurnaceOnTrigger inject.
// Prior pure port: slice 1149. Residual dual-wire suite: 2877 /
// test_synergy_claim_furnace_2877. Prior dedicated dual-wire suite:
// test_synergy_claim_furnace_3065. Prior dedicated expand residual suites:
// test_synergy_claim_furnace_3239, test_synergy_claim_furnace_3285,
// test_synergy_claim_furnace_3315, test_synergy_claim_furnace_3494,
// test_synergy_claim_furnace_3551, test_synergy_claim_furnace_3596,
// test_synergy_claim_furnace_3641, test_synergy_claim_furnace_3686,
// test_synergy_claim_furnace_3731, test_synergy_claim_furnace_3776,
// test_synergy_claim_furnace_3821.
// Dedicated expand residual suite is test_synergy_claim_furnace_3866.
// Host still owns message / attach / timers after a true gate.
// Formula is unchanged.
inline auto CanClaimFurnace(const uint8 state) -> bool
{
    return state == FurnaceAvailable;
}

// IsClaimedBy is the pure equality half of claimedByYou checks
// (synergyFurnacePlayerID matches player ID). Zero furnace player ID is
// never a claim owner (matches Go synergy.IsClaimedBy).
inline auto IsClaimedBy(const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return furnacePlayerID != 0 && furnacePlayerID == playerID;
}

// ---------------------------------------------------------------------------
// Slice 2896 residual / 3098 prior dedicated / 3359 dedicated expand residual
// 2896 — synergyFurnaceOnTrade CLAIMED+owner gate
// ---------------------------------------------------------------------------

// CanTradeIntoFurnace mirrors the pure CLAIMED+owner gate of
// synergyFurnaceOnTrade after ENABLE_SYNERGY is checked by the host:
//   if furnaceState ~= furnaceStates.CLAIMED then return end
//   local claimedByYou = furnacePlayerID == player:getID()
//   if not claimedByYou then return end
//   -- recipe lookup / fewell packing / startEvent 4521 ...
//
// Formula (slice 3359 dual-wire expand residual 2896; prior dedicated 3098):
//   CanTradeIntoFurnace(state, furnacePlayerID, playerID)
//     = state == FurnaceClaimed && IsClaimedBy(furnacePlayerID, playerID)
//
// state / furnacePlayerID are host-injected npc local vars
// (synergyFurnaceState / synergyFurnacePlayerID).
// true  → host recipe lookup / fewell packing / startEvent 4521
// false → early return (wrong state / not owner)
//
// Dual-wire of Go synergy.CanTradeIntoFurnace.
// Call site: future Lua synergyFurnaceOnTrade inject.
// Prior pure port: slice 1149. Residual dual-wire suite: 2896 /
// test_synergy_trade_furnace_2896. Prior dedicated dual-wire suite:
// test_synergy_trade_furnace_3098. Dedicated expand residual suite is
// test_synergy_trade_furnace_3359. Host still owns ENABLE_SYNERGY /
// recipe / trade consume / fewell / startEvent after a true gate.
// Formula is unchanged.
inline auto CanTradeIntoFurnace(const uint8 state, const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return state == FurnaceClaimed && IsClaimedBy(furnacePlayerID, playerID);
}

// ---------------------------------------------------------------------------
// Slice 2899 residual / 3117 prior dedicated / 3383 prior dedicated expand
// residual 2899 / 3439 dedicated expand residual 2899
// — synergyFurnaceOnTrigger claimedByYou operate gate
// ---------------------------------------------------------------------------

// CanOperateFurnace mirrors the pure claimedByYou gate shared by
// synergyFurnaceOnTrigger CLAIMED / ACTIVE / COMPLETED branches:
//   local claimedByYou = furnacePlayerID == player:getID()
//   if not claimedByYou then return end
//   -- CLAIMED: startEvent 4520 fewell menu
//   -- ACTIVE: distance check then startEvent 4518
//   -- COMPLETED: retrieve / startEvent path
//
// Formula (slice 3439 dual-wire expand residual 2899; prior dedicated expand
// 3383; prior dedicated 3117):
//   CanOperateFurnace(furnacePlayerID, playerID)
//     = IsClaimedBy(furnacePlayerID, playerID)
//   // furnacePlayerID != 0 && furnacePlayerID == playerID
//
// furnacePlayerID is the host-injected npc local-var synergyFurnacePlayerID.
// Zero furnace player ID is never an owner (via IsClaimedBy).
// true  → host CLAIMED menu / ACTIVE distance+operate / COMPLETED retrieve
// false → early return (not owner)
//
// Dual-wire of Go synergy.CanOperateFurnace.
// Call site: future Lua synergyFurnaceOnTrigger inject.
// Prior pure port: slice 1149. Residual dual-wire suite: 2899 /
// test_synergy_operate_furnace_2899. Prior dedicated dual-wire suite:
// test_synergy_operate_furnace_3117. Prior dedicated expand residual suite:
// test_synergy_operate_furnace_3383. Dedicated expand residual suite is
// test_synergy_operate_furnace_3439. Host still owns state branch
// dispatch, distance checks, startEvent CSIDs, and retrieve side
// effects after a true gate.
// Formula is unchanged.
inline auto CanOperateFurnace(const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return IsClaimedBy(furnacePlayerID, playerID);
}

} // namespace synergyhelpers
