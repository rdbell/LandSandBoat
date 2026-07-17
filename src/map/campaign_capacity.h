#pragma once

#include "common/cbasetypes.h"

// Pure campaign helpers shared by dual-wire slices:
//   - 2858: ShouldDebitBonusCost (sigil apply delCurrency gate)
//   - 2946: CanAffordAlliedNotes residual dual-wire suite
//   - 3072: CanAffordAlliedNotes dedicated dual-wire (afford_notes.go)
//
// Dual-wire index:
//   - 2858: ShouldDebitBonusCost (bonusCost > 0)
//   - 2946: CanAffordAlliedNotes residual dual-wire suite
//   - 3072: CanAffordAlliedNotes (notes >= price)
//
// Production host is Lua scripts/globals/campaign.lua sigilOnEventFinish.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / currency pointers). Side effects (giveItem,
// delCurrency) remain host-owned.
// Go dual-wire: campaign.CanAffordAlliedNotes
// (internal/campaign/afford_notes.go). Future Lua host injects
// CanAffordAlliedNotes then giveItem / delCurrency.
//
// Prior pure port: OmegaXI slice 1115 (internal/campaign flow.go).

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

// ---------------------------------------------------------------------------
// Slice 2946 / 3072 — sigil shop purchase afford gate
// ---------------------------------------------------------------------------

// CanAffordAlliedNotes mirrors the sigil shop purchase afford gate:
//   notes >= price
// Positive form of:
//   if player:getCurrency('allied_notes') < itemPrice then return end
//
// Formula (slice 3072 dual-wire; residual expand 2946 / pure 1115 — formula
// unchanged):
//   CanAffordAlliedNotes(notes, price) = notes >= price
//
// Host injects notes (getCurrency) and price (itemPrice after optional *1.5).
// Exact notes (notes == price) affords; free items (price == 0) always afford
// when notes >= 0.
//
// Dual-wire of Go campaign.CanAffordAlliedNotes (afford_notes.go).
// Call site: future Lua sigilOnEventFinish inject.
// Prior pure port: slice 1115. Residual dual-wire suite: 2946 /
// test_campaign_afford_notes_2946. Dedicated dual-wire suite is
// test_campaign_afford_notes_3072. Host still owns giveItem / delCurrency
// after a true gate.
inline auto CanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace campaignhelpers
