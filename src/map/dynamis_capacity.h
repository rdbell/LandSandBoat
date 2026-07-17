#pragma once

// Pure Dynamis zoneOnZoneIn origin-snap policy helpers (slice 2857).
//
// Lua production host: scripts/globals/dynamis.lua xi.dynamis.zoneOnZoneIn
// elseif branch after the DYNAMIS-status check:
//
//   elseif getXPos()==0 and getYPos()==0 and getZPos()==0 then
//     setPos(unpack(info.entryPos))
//
// Host injects scalars only (no entity pointers):
//   hasDynamisEffect — hasStatusEffect(DYNAMIS) after optional entry setup
//   x, y, z          — player getXPos / getYPos / getZPos
//
// setPos(entryPos) writeback remains host-owned.

namespace dynamishelpers
{

// AtOrigin reports exact float equality of position to (0, 0, 0).
// Matches LSB Lua `getXPos()==0 and getYPos()==0 and getZPos()==0`.
inline auto AtOrigin(const float x, const float y, const float z) -> bool
{
    return x == 0.0f && y == 0.0f && z == 0.0f;
}

// ShouldSnapToEntryPos is the pure free-function form of the origin-snap gate:
//   hasDynamisEffect && AtOrigin(x, y, z)
// Requires effective DYNAMIS (post entry-setup or pre-existing).
inline auto ShouldSnapToEntryPos(const bool hasDynamisEffect, const float x, const float y, const float z) -> bool
{
    return hasDynamisEffect && AtOrigin(x, y, z);
}

} // namespace dynamishelpers
