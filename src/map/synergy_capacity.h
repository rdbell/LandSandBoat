#pragma once

#include "common/cbasetypes.h"

// Pure Synergy furnace helpers shared by dual-wire slices:
//   - 2877: CanClaimFurnace (synergyFurnaceOnTrigger AVAILABLE gate)
//   - 2896: CanTradeIntoFurnace (synergyFurnaceOnTrade CLAIMED+owner gate)
//
// Production hosts are Lua under scripts/globals/synergy.lua
// (furnaceStates + synergyFurnaceOnTrigger / synergyFurnaceOnTrade).
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining state == AVAILABLE / CLAIMED and
// claimedByYou. Helpers take host-injected scalars only (no player /
// npc / entity pointers). Side effects (CLAIM_SET message,
// attachToSynergyFurnace, distance checker, ENABLE_SYNERGY, recipe
// lookup, trade consume, startEvent) remain host-owned.
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

// CanClaimFurnace mirrors the pure AVAILABLE gate of synergyFurnaceOnTrigger
// before CLAIM_SET + attachToSynergyFurnace:
//   if furnaceState == furnaceStates.AVAILABLE then
//     player:messageSpecial(CLAIM_SET, SYNERGY_CRUCIBLE)
//     xi.synergy.attachToSynergyFurnace(player, npc)
//   end
// state is the host-injected npc:getLocalVar(synergyFurnaceState).
// Host still owns message / attach / timers.
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

// CanTradeIntoFurnace mirrors the pure CLAIMED+owner gate of
// synergyFurnaceOnTrade after ENABLE_SYNERGY is checked by the host:
//   if furnaceState ~= furnaceStates.CLAIMED then return end
//   local claimedByYou = furnacePlayerID == player:getID()
//   if not claimedByYou then return end
// state / furnacePlayerID are host-injected npc local vars.
// Host still owns recipe lookup / trade consume / fewell packing /
// startEvent 4521.
inline auto CanTradeIntoFurnace(const uint8 state, const uint32 furnacePlayerID, const uint32 playerID) -> bool
{
    return state == FurnaceClaimed && IsClaimedBy(furnacePlayerID, playerID);
}

} // namespace synergyhelpers
