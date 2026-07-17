#pragma once

#include "common/cbasetypes.h"

// Pure campaign helpers (slices 2858, 2946).
//
// Production host is Lua scripts/globals/campaign.lua sigilOnEventFinish.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure gates instead
// of re-inlining comparisons. Helpers take host-injected scalars only
// (no entity / currency pointers).

namespace campaignhelpers
{

// ShouldDebitBonusCost mirrors the sigil apply delCurrency gate (slice 2858):
//   bonusCost > 0
// Host injects bonusCost from SigilBonusCost / selected-effect loop.
// Zero cost skips delCurrency('allied_notes', bonusCost).
inline auto ShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

// CanAffordAlliedNotes mirrors the sigil shop purchase afford gate (slice 2946):
//   notes >= price
// Positive form of:
//   if player:getCurrency('allied_notes') < itemPrice then return end
// Host injects notes (getCurrency) and price (itemPrice after optional *1.5).
// Exact notes (notes == price) affords; free items (price == 0) always afford.
inline auto CanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace campaignhelpers
