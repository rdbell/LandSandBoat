#pragma once

#include "common/cbasetypes.h"

// Pure Synergy furnace helpers shared by dual-wire slices:
//   - 2877: CanClaimFurnace residual dual-wire suite (AVAILABLE gate)
//   - 2896: CanTradeIntoFurnace residual dual-wire suite (CLAIMED+owner)
//   - 2899: CanOperateFurnace (synergyFurnaceOnTrigger claimedByYou gate)
//   - 3065: CanClaimFurnace dedicated dual-wire (claim_furnace.go)
//   - 3098: CanTradeIntoFurnace dedicated dual-wire (trade_furnace.go)
//
// Dual-wire index:
//   - 2877: CanClaimFurnace residual dual-wire suite
//   - 2896: CanTradeIntoFurnace residual dual-wire suite
//   - 2899: CanOperateFurnace (IsClaimedBy alias)
//   - 3065: CanClaimFurnace (state == FurnaceAvailable)
//   - 3098: CanTradeIntoFurnace (state == FurnaceClaimed && IsClaimedBy)
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
//
// Prior pure port: OmegaXI slice 1149 (internal/synergy furnace.go).

namespace synergyhelpers
{

// furnaceStates.AVAILABLE — npc local-var synergyFurnaceState when free.
// Matches Go synergy.FurnaceAvailable and Lua furnaceStates.AVAILABLE.
constexpr uint8 FurnaceAvailable = 0;

// furnaceStates.CLAIMED — npc local-var synergyFurnaceState when claimed.
// Matches Go synergy.FurnaceClaimed and Lua furnaceStates.CLAIMED.
constexpr uint8 FurnaceClaimed = 1;

// ---------------------------------------------------------------------------
// Slice 2877 / 3065 — synergyFurnaceOnTrigger AVAILABLE claim gate
// ---------------------------------------------------------------------------

// CanClaimFurnace mirrors the pure AVAILABLE gate of synergyFurnaceOnTrigger
// before CLAIM_SET + attachToSynergyFurnace:
//   if furnaceState == furnaceStates.AVAILABLE then
//     player:messageSpecial(CLAIM_SET, SYNERGY_CRUCIBLE)
//     xi.synergy.attachToSynergyFurnace(player, npc)
//   end
//
// Formula (slice 3065 dual-wire; residual expand 2877):
//   CanClaimFurnace(state) = state == FurnaceAvailable
//
// state is the host-injected npc:getLocalVar(synergyFurnaceState).
// true  → host CLAIM_SET + attachToSynergyFurnace
// false → other handleFurnaceState branches (CLAIMED / ACTIVE / COMPLETED)
//
// Dual-wire of Go synergy.CanClaimFurnace.
// Call site: future Lua synergyFurnaceOnTrigger inject.
// Prior pure port: slice 1149. Residual dual-wire suite: 2877 /
// test_synergy_claim_furnace_2877. Dedicated dual-wire suite is
// test_synergy_claim_furnace_3065. Host still owns message / attach /
// timers after a true gate.
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
// Slice 2896 / 3098 — synergyFurnaceOnTrade CLAIMED+owner gate
// ---------------------------------------------------------------------------

// CanTradeIntoFurnace mirrors the pure CLAIMED+owner gate of
// synergyFurnaceOnTrade after ENABLE_SYNERGY is checked by the host:
//   if furnaceState ~= furnaceStates.CLAIMED then return end
//   local claimedByYou = furnacePlayerID == player:getID()
//   if not claimedByYou then return end
//   -- recipe lookup / fewell packing / startEvent 4521 ...
//
// Formula (slice 3098 dual-wire; residual expand 2896):
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
// test_synergy_trade_furnace_2896. Dedicated dual-wire suite is
// test_synergy_trade_furnace_3098. Host still owns ENABLE_SYNERGY /
// recipe / trade consume / fewell / startEvent after a true gate.
inline auto CanTradeIntoFurnace(const uint8 state, const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return state == FurnaceClaimed && IsClaimedBy(furnacePlayerID, playerID);
}

// CanOperateFurnace mirrors the pure claimedByYou gate shared by
// synergyFurnaceOnTrigger CLAIMED / ACTIVE / COMPLETED branches:
//   local claimedByYou = furnacePlayerID == player:getID()
//   if not claimedByYou then return end
// furnacePlayerID is the host-injected npc local-var synergyFurnacePlayerID.
// Zero furnace player ID is never an owner (via IsClaimedBy).
// Host still owns state branch dispatch, distance checks, startEvent
// CSIDs, and retrieve side effects.
// Matches Go synergy.CanOperateFurnace:
//   IsClaimedBy(furnacePlayerID, playerID)
//   // furnacePlayerID != 0 && furnacePlayerID == playerID
inline auto CanOperateFurnace(const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return IsClaimedBy(furnacePlayerID, playerID);
}

} // namespace synergyhelpers
