#pragma once

#include "common/cbasetypes.h"

// Pure seals/crests helpers for dual-wire slices:
//   - 2886: PackStoredPlusTrade residual dual-wire expand
//   - 3172: PackStoredPlusTrade dedicated dual-wire
//           ((storedSeals + itemCount) << 16;
//            residual expand 2886 / pure 0961)
//
// Lua production host: scripts/globals/seals.lua
//   xi.seals.onTrade packs event params as:
//     eventParams[sealID + 2] = bit.lshift(storedSeals + itemCount, 16)
//   where:
//     storedSeals = player:getSeals(sealID)
//     itemCount   = sealOption[2]  -- trade:getItemQty
//
// Host injects scalars only (no player / trade pointers):
//   storedSeals — player:getSeals(sealID)
//   itemCount   — trade quantity of the seal/crest item
//
// getSeals/addSeals, startEvent, confirmItem/confirmTrade remain host-owned.
// Prior pure port: OmegaXI slice 0961 (internal/seals).
// Residual dual-wire suite: 2886 / test_seals_pack_trade_2886.
// Dedicated dual-wire suite: 3172 / test_seals_pack_stored_plus_trade_3172.
// Go dual-wire: seals.PackStoredPlusTrade
// (internal/seals/pack_stored_plus_trade.go).

namespace sealshelpers
{

// PackStoredPlusTrade builds the high-half event param word used by onTrade:
//   (storedSeals + itemCount) << 16
// Hosts assign the result to eventParams[sealID + 2].
//
// Formula (slice 3172 dedicated dual-wire; residual expand 2886 / pure 0961 —
// formula unchanged):
//   (storedSeals + itemCount) << 16
//
// Dual-wire of Go seals.PackStoredPlusTrade
// (internal/seals/pack_stored_plus_trade.go). Prior pure port: slice 0961.
// Residual dual-wire suite: 2886 / test_seals_pack_trade_2886.
// Dedicated dual-wire suite is test_seals_pack_stored_plus_trade_3172. Formula is
// unchanged; this slice only expands dual-wire docs + index + dedicated suite.
// Production Lua still inlines bit.lshift until host inject lands.
inline auto PackStoredPlusTrade(const int32 storedSeals, const int32 itemCount) -> int32
{
    return (storedSeals + itemCount) << 16;
}

} // namespace sealshelpers
