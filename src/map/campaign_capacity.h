#pragma once

#include "common/cbasetypes.h"

// Pure campaign helpers (slice 2858).
//
// Production host is Lua scripts/globals/campaign.lua sigilOnEventFinish
// (if bonusCost > 0 then delCurrency). Capacity is for future Lua/C++ inject
// so hosts dual-wire the pure gate instead of re-inlining the comparison.
// Helpers take host-injected scalars only (no entity / currency pointers).

namespace campaignhelpers
{

// ShouldDebitBonusCost mirrors the sigil apply delCurrency gate:
//   bonusCost > 0
// Host injects bonusCost from SigilBonusCost / selected-effect loop.
// Zero cost skips delCurrency('allied_notes', bonusCost).
inline auto ShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

} // namespace campaignhelpers
