#pragma once

#include "common/cbasetypes.h"

// Pure campaign helpers shared by dual-wire slices:
//   - 2858: ShouldDebitBonusCost residual dual-wire suite
//   - 3103: ShouldDebitBonusCost prior dedicated dual-wire (retained)
//   - 3357: ShouldDebitBonusCost dedicated dual-wire (debit_bonus_cost.go;
//           expand residual 2858)
//   - 3141: ShouldDebitSelectedEffects prior dedicated dual-wire (retained)
//   - 3397: ShouldDebitSelectedEffects prior dedicated dual-wire (retained)
//   - 3441: ShouldDebitSelectedEffects dedicated dual-wire
//           (debit_selected_effects.go; expand residual 3141)
//   - 2946: CanAffordAlliedNotes residual dual-wire suite
//   - 3072: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3226: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3273: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3304: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3496: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3553: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3598: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3643: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3688: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3733: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3778: CanAffordAlliedNotes dedicated dual-wire (afford_notes.go;
//           expand residual 2946)
//
// Dual-wire index:
//   - 2858: ShouldDebitBonusCost residual dual-wire suite
//   - 3103: ShouldDebitBonusCost prior dedicated dual-wire (retained)
//   - 3357: ShouldDebitBonusCost (bonusCost > 0)
//   - 3141: ShouldDebitSelectedEffects prior dedicated dual-wire (retained)
//   - 3397: ShouldDebitSelectedEffects prior dedicated dual-wire (retained)
//   - 3441: ShouldDebitSelectedEffects
//           (ShouldDebitBonusCost(SigilBonusCost(selectedEffects)))
//   - 2946: CanAffordAlliedNotes residual dual-wire suite
//   - 3072: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3226: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3273: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3304: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3496: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3553: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3598: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3643: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3688: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3733: CanAffordAlliedNotes prior dedicated dual-wire (retained)
//   - 3778: CanAffordAlliedNotes (notes >= price)
//
// Production host is Lua scripts/globals/campaign.lua sigilOnEventFinish.
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / currency pointers). Side effects (giveItem,
// delCurrency) remain host-owned.
// Go dual-wire: campaign.ShouldDebitBonusCost
// (internal/campaign/debit_bonus_cost.go), campaign.ShouldDebitSelectedEffects
// (internal/campaign/debit_selected_effects.go), and
// campaign.CanAffordAlliedNotes (internal/campaign/afford_notes.go). Future
// Lua host injects free functions then giveItem / delCurrency.
//
// Prior pure port: OmegaXI slice 1115 (internal/campaign flow.go).

namespace campaignhelpers
{

// ---------------------------------------------------------------------------
// Slice 2858 residual / 3103 prior dedicated / 3357 dedicated —
// sigil apply delCurrency debit gate
// ---------------------------------------------------------------------------

// ShouldDebitBonusCost mirrors the sigil apply delCurrency gate:
//   bonusCost > 0
// Host injects bonusCost from SigilBonusCost / selected-effect loop.
// Zero cost skips delCurrency('allied_notes', bonusCost).
//
// Formula (slice 3357 dedicated dual-wire expand residual 2858; prior dedicated
// 3103 / pure 1115 — formula unchanged):
//   ShouldDebitBonusCost(bonusCost) = bonusCost > 0
//
// Dual-wire of Go campaign.ShouldDebitBonusCost (debit_bonus_cost.go).
// Call site: future Lua sigilOnEventFinish inject.
// Prior pure port: slice 1115. Residual dual-wire suite: 2858 /
// test_campaign_debit_bonus_2858. Prior dedicated dual-wire suite: 3103 /
// test_campaign_debit_bonus_3103 (retained). Dedicated dual-wire suite is
// test_campaign_debit_bonus_3357. Host still owns delCurrency after a true gate.
// Sibling 3441 / prior 3397 / 3141 ShouldDebitSelectedEffects composes this
// gate; leave alone (do not thrash debit_bonus_cost).
inline auto ShouldDebitBonusCost(const int32 bonusCost) -> bool
{
    return bonusCost > 0;
}

// ---------------------------------------------------------------------------
// Slice 1115 residual / 3441 compose dependency — selected-effect bonus cost
// ---------------------------------------------------------------------------

// SigilBonusCost mirrors the Lua bits-1..4 × 50 loop (slice 1115 pure):
//   bonusCost = 0
//   for i = 1, 4:
//     if selectedEffects bit i set: bonusCost += 50
// Bit 0 (Regen per comments) is intentionally not billed.
// Dual-wire dependency for ShouldDebitSelectedEffects (3441 / prior 3397 /
// 3141); formula unchanged from Go campaign.SigilBonusCost (flow.go).
inline auto SigilBonusCost(const int32 selectedEffects) -> int32
{
    int32 cost = 0;
    for (int32 i = 1; i <= 4; ++i)
    {
        if ((selectedEffects & (1 << i)) != 0)
        {
            cost += 50;
        }
    }
    return cost;
}

// ---------------------------------------------------------------------------
// Slice 3141 residual / 3397 prior dedicated / 3441 dedicated — apply-path
// selected-effects debit compose
// ---------------------------------------------------------------------------

// ShouldDebitSelectedEffects dual-wires the apply-path debit gate from
// selectedEffects (option>>11):
//
// Formula (slice 3441 dedicated dual-wire expand residual 3141; prior dedicated
// 3397 / pure 1115 / residual compose 2858 — formula unchanged):
//   ShouldDebitSelectedEffects(selectedEffects) =
//     ShouldDebitBonusCost(SigilBonusCost(selectedEffects))
//
// Which means debit when SigilBonusCost(selectedEffects) > 0.
// Sibling ShouldDebitBonusCost (3357 / prior 3103) is called, not reimplemented.
//
// Dual-wire of Go campaign.ShouldDebitSelectedEffects
// (debit_selected_effects.go). Call site: future Lua sigilOnEventFinish
// inject. Prior dedicated dual-wire suites: 3141 /
// test_campaign_debit_selected_3141 and 3397 /
// test_campaign_debit_selected_3397 (retained). Dedicated dual-wire suite:
// test_campaign_debit_selected_3441. Host still owns delCurrency after a true
// gate.
inline auto ShouldDebitSelectedEffects(const int32 selectedEffects) -> bool
{
    return ShouldDebitBonusCost(SigilBonusCost(selectedEffects));
}

// ---------------------------------------------------------------------------
// Slice 2946 residual / 3072+3226+3273+3304+3496+3553+3598+3643+3688+3733 prior dedicated / 3778 dedicated —
// sigil shop purchase afford gate
// ---------------------------------------------------------------------------

// CanAffordAlliedNotes mirrors the sigil shop purchase afford gate:
//   notes >= price
// Positive form of:
//   if player:getCurrency('allied_notes') < itemPrice then return end
//
// Formula (slice 3778 dedicated dual-wire expand residual 2946; prior dedicated
// 3733 / 3688 / 3643 / 3598 / 3553 / 3496 / 3304 / 3273 / pure 1115 — formula unchanged):
//   CanAffordAlliedNotes(notes, price) = notes >= price
//
// Host injects notes (getCurrency) and price (itemPrice after optional *1.5).
// Exact notes (notes == price) affords; free items (price == 0) always afford
// when notes >= 0.
//
// Dual-wire of Go campaign.CanAffordAlliedNotes (afford_notes.go).
// Call site: future Lua sigilOnEventFinish inject.
// Prior pure port: slice 1115. Residual dual-wire suite: 2946 /
// test_campaign_afford_notes_2946. Prior dedicated dual-wire suites: 3072 /
// test_campaign_afford_notes_3072, 3226 / test_campaign_afford_notes_3226,
// 3273 / test_campaign_afford_notes_3273, 3304 /
// test_campaign_afford_notes_3304, 3496 /
// test_campaign_afford_notes_3496, 3553 /
// test_campaign_afford_notes_3553, 3598 /
// test_campaign_afford_notes_3598, 3643 /
// test_campaign_afford_notes_3643, 3688 /
// test_campaign_afford_notes_3688, and 3733 /
// test_campaign_afford_notes_3733 (retained). Dedicated dual-wire suite
// is test_campaign_afford_notes_3778.
// Host still owns giveItem / delCurrency after a true gate.
inline auto CanAffordAlliedNotes(const int32 notes, const int32 price) -> bool
{
    return notes >= price;
}

} // namespace campaignhelpers
