#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure Porter Moogle dual-wire helpers.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1144: residual pure port (internal/portermoogle flow helpers)
//   - 2966: CanRetrieveItem residual dual-wire suite (hasItem / freeSlots gate)
//   - 3171: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966)
//   - 3243: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior 3171)
//   - 3286: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior ~3243)
//   - 3316: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior ~3286)
//   - 3409: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior ~3316)
//   - 3463: CanRetrieveItem prior dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior ~3409)
//   - 3512: CanRetrieveItem dedicated dual-wire (can_retrieve_item.go;
//           expand residual 2966 / prior ~3463)
//
// Dual-wire index:
//   - 2966: CanRetrieveItem residual dual-wire suite
//   - 3171: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3243: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3286: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3316: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3409: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3463: CanRetrieveItem = !hasItem && freeSlots > 0 (prior dedicated)
//   - 3512: CanRetrieveItem = !hasItem && freeSlots > 0
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
// Residual dual-wire suite: 2966 (test_porter_retrieve_item_2966).
// Prior dedicated dual-wire suites: 3171
// (test_portermoogle_can_retrieve_item_3171), 3243
// (test_portermoogle_can_retrieve_item_3243), 3286
// (test_portermoogle_can_retrieve_item_3286), 3316
// (test_portermoogle_can_retrieve_item_3316), 3409
// (test_portermoogle_can_retrieve_item_3409), 3463
// (test_portermoogle_can_retrieve_item_3463).
// Dedicated dual-wire suite: 3512 (test_portermoogle_can_retrieve_item_3512).

namespace portermooglehelpers
{

// ---------------------------------------------------------------------------
// Slice 3512 — CanRetrieveItem (dedicated expand residual 2966 / prior ~3463)
// ---------------------------------------------------------------------------

// CanRetrieveItem mirrors the Lua porter_moogle free-slot / already-owned
// gate pure half.
//
// Formula (slice 3512 dedicated dual-wire; residual expand 2966 / prior
// dedicated ~3463 / 3409 / 3316 / 3286 / 3243 / 3171 / pure 1144 — formula unchanged):
//
//   !hasItem && freeSlots > 0
//
// Equivalently can-retrieve when NOT (hasItem OR freeSlots == 0) for
// non-negative free-slot counts (production getFreeSlotsCount is >= 0;
// freeSlots > 0 rejects zero and negatives). Matches Go
// portermoogle.CanRetrieveItem (residual 1144 / residual dual-wire 2966 /
// prior dedicated dual-wire 3171 / 3243 / 3286 / 3316 / 3409 / 3463 / dedicated
// dual-wire 3512).
//
// hasItem   — host-injected player:hasItem(itemId)
// freeSlots — host-injected player:getFreeSlotsCount()
// true  → open retrieve (clear bit / give item path)
// false → cannot obtain (messageSpecial host path)
//
// Dual-wire of Go portermoogle.CanRetrieveItem.
// Prior pure port: slice 1144. Residual dual-wire suite: 2966 /
// test_porter_retrieve_item_2966. Prior dedicated dual-wire suites are
// test_portermoogle_can_retrieve_item_3171,
// test_portermoogle_can_retrieve_item_3243,
// test_portermoogle_can_retrieve_item_3286,
// test_portermoogle_can_retrieve_item_3316,
// test_portermoogle_can_retrieve_item_3409, and
// test_portermoogle_can_retrieve_item_3463. Dedicated dual-wire suite is
// test_portermoogle_can_retrieve_item_3512.
inline auto CanRetrieveItem(const bool hasItem, const int32 freeSlots) -> bool
{
    return !hasItem && freeSlots > 0;
}

} // namespace portermooglehelpers
