#pragma once

// Pure Dynamis helpers shared by dual-wire slices:
//   - 2857 residual: AtOrigin (zoneOnZoneIn origin equality half)
//   - 3078: ShouldSnapToEntryPos (zoneOnZoneIn origin snap gate)
//   - 2921: CanUnlockSJ (somnial threshold startEvent param)
//
// Lua production hosts: scripts/globals/dynamis.lua
//   - xi.dynamis.zoneOnZoneIn (origin snap elseif after DYNAMIS check)
//   - xi.dynamis.somnialThresholdOnTrigger (canUnlockSJ event param)
//
// Host injects scalars only (no entity pointers). Entity writeback
// (setPos / startEvent / delStatusEffectSilent) remains host-owned.
//
// Index 3078: dynamis.ShouldSnapToEntryPos pure dual-wire.
// Go dual-wire: dynamis.ShouldSnapToEntryPos (internal/dynamis/snap_entry.go).

namespace dynamishelpers
{

// ---------------------------------------------------------------------------
// Origin-snap (AtOrigin residual 2857; ShouldSnapToEntryPos slice 3078)
// ---------------------------------------------------------------------------

// AtOrigin reports exact float equality of position to (0, 0, 0).
// Matches LSB Lua `getXPos()==0 and getYPos()==0 and getZPos()==0`.
// Residual pure helper (slice 2857); used by ShouldSnapToEntryPos (3078).
inline auto AtOrigin(const float x, const float y, const float z) -> bool
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

// ShouldSnapToEntryPos is the pure free-function form of the origin-snap gate.
//
// Formula (slice 3078 dual-wire):
//   hasDynamisEffect && AtOrigin(x, y, z)
//
// hasDynamisEffect — host-evaluated effective DYNAMIS status
//                    (post entry-setup or pre-existing)
// x,y,z            — player position (exact float equality to origin)
// true  → host setPos(unpack(info.entryPos))
// false → leave position alone (or eject CS when no DYNAMIS)
//
// Dual-wire of Go dynamis.ShouldSnapToEntryPos (snap_entry.go).
// Call site: future Lua host inject of zoneOnZoneIn origin-snap branch;
// OmegaXI ResolveZoneIn.SnapToEntryPos dual-wires this free function.
// Prior pure port: slice 1119; residual dual-wire notes: slice 2857.
// Index 3078: dynamis.ShouldSnapToEntryPos pure dual-wire.
inline auto ShouldSnapToEntryPos(const bool hasDynamisEffect, const float x, const float y, const float z) -> bool
{
    return hasDynamisEffect && AtOrigin(x, y, z);
}

// ---------------------------------------------------------------------------
// Somnial CanUnlockSJ (slice 2921)
// ---------------------------------------------------------------------------

// CanUnlockSJ is the pure free-function form of the somnial threshold
// startEvent param from xi.dynamis.somnialThresholdOnTrigger:
//
//   local canUnlockSJ = player:hasStatusEffect(xi.effect.SJ_RESTRICTION) and 1 or 0
//   player:startEvent(101, 0x27, canUnlockSJ, menuBits)
//
// Host injects hasStatusEffect(SJ_RESTRICTION) as a bool. Returns 1 when the
// player has SJ restriction (can unlock support jobs), else 0.
// Dual-wires OmegaXI internal/dynamis CanUnlockSJ (unlock_sj.go).
inline auto CanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

} // namespace dynamishelpers
