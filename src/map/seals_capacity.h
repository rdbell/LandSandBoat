#pragma once

#include "common/cbasetypes.h"

// Pure seals/crests helpers for dual-wire slices:
//   - 2886: PackStoredPlusTrade ((storedSeals + itemCount) << 16)
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
// Dual-wire of Go seals.PackStoredPlusTrade (slice 2886).

namespace sealshelpers
{

// PackStoredPlusTrade builds the high-half event param word used by onTrade:
//   (storedSeals + itemCount) << 16
// Hosts assign the result to eventParams[sealID + 2].
// Dual-wire of Go seals.PackStoredPlusTrade / Lua bit.lshift(stored + count, 16).
inline auto PackStoredPlusTrade(const int32 storedSeals, const int32 itemCount) -> int32
{
    return (storedSeals + itemCount) << 16;
}

} // namespace sealshelpers
