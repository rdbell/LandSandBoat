#pragma once

#include "common/cbasetypes.h"

// Pure Synergy furnace helpers shared by dual-wire slices:
//   - 2877: CanClaimFurnace (synergyFurnaceOnTrigger AVAILABLE gate)
//
// Production hosts are Lua under scripts/globals/synergy.lua
// (furnaceStates + synergyFurnaceOnTrigger handleFurnaceState[AVAILABLE]).
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining state == AVAILABLE. Helpers take
// host-injected scalars only (no player / npc / entity pointers).
// Side effects (CLAIM_SET message, attachToSynergyFurnace, distance
// checker, ENABLE_SYNERGY) remain host-owned.
//
// Prior pure port: OmegaXI slice 1149 (internal/synergy furnace.go).

namespace synergyhelpers
{

// furnaceStates.AVAILABLE — npc local-var synergyFurnaceState when free.
// Matches Go synergy.FurnaceAvailable and Lua furnaceStates.AVAILABLE.
constexpr uint8 FurnaceAvailable = 0;

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

} // namespace synergyhelpers
