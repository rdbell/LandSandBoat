#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Porter Moogle dual-wire helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1144: residual pure port (internal/portermoogle flow helpers)
//   - 2966: CanRetrieveItem (hasItem / freeSlots retrieve gate)
//
// Lua production host: scripts/globals/porter_moogle.lua
//   onEventUpdate retrieve path:
//     if player:hasItem(retrievedItemId) or player:getFreeSlotsCount() == 0 then
//         messageSpecial(ITEM_CANNOT_BE_OBTAINED, retrievedItemId)
//     else
//         -- clear bit / retrieve
//     end
//   onEventFinish buy-slip path uses the same scalar form for slip ownership
//   + free slots before delGil / addItem.
//
// This capacity dual-wires the free-function form used by OmegaXI
// internal/portermoogle (can_retrieve_item.go) so hosts call CanRetrieveItem
// instead of re-inlining:
//
//   !hasItem && freeSlots > 0
//
// Hosts inject hasItem / freeSlots scalars only (no CBaseEntity* / player*).
// messageSpecial / updateEvent / retrieveItemFromSlip / delGil writeback
// remains host-owned.
//
// Go dual-wire: portermoogle.CanRetrieveItem
// (internal/portermoogle/can_retrieve_item.go).

namespace portermooglehelpers
{

// CanRetrieveItem mirrors the Lua porter_moogle free-slot / already-owned
// gate pure half (slice 2966):
//
//   !hasItem && freeSlots > 0
//
// Equivalently can-retrieve when NOT (hasItem OR freeSlots == 0) for
// non-negative free-slot counts (production getFreeSlotsCount is >= 0;
// freeSlots > 0 rejects zero and negatives). Matches Go
// portermoogle.CanRetrieveItem (residual 1144 / dual-wire 2966).
//
// hasItem   — host-injected player:hasItem(itemId)
// freeSlots — host-injected player:getFreeSlotsCount()
// true  → open retrieve (clear bit / give item path)
// false → cannot obtain (messageSpecial host path)
inline auto CanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

} // namespace portermooglehelpers
