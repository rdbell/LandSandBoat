#include "test_treasurepool_lot_rare_3291.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasurepool CanLotRareItem 3291 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem rare-owned formula for dual-wire cross-check (dedicated 3291):
//   !itemIsRare || !alreadyHasItem  (positive form only; QF1001)
auto inlineCanLotRareItem(const bool itemIsRare, const bool alreadyHasItem) -> bool
{
    return !itemIsRare || !alreadyHasItem;
}

// Compact dual-wire pin matching Go pinCanLotRareItem3291 / positive form:
//   !itemIsRare || !alreadyHasItem
auto pinCanLotRareItem(const bool itemIsRare, const bool alreadyHasItem) -> bool
{
    return !itemIsRare || !alreadyHasItem;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::CanLotRareItem
// (!itemIsRare || !alreadyHasItem rare-owned lot gate; OmegaXI
// internal/treasurepool; dedicated slice 3291 expand residual 2998;
// prior dedicated ~3260).
//
// Coverage:
//   - free == inline == pin == (!itemIsRare || !alreadyHasItem)  [positive form]
//   - residual 1367 / 2998 / prior 3260 pins still hold
//   - residual poles + dense 2² over both bool inputs
auto runTreasurepoolLotRare3291SelfTests() -> bool
{
    using treasurepoolhelpers::CanLotRareItem;
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    bool ok = true;

    // Residual 1367 / 2998 / prior 3260 pins still hold under dual-wire.
    ok = expect(CanLotRareItem(false, false), "residual non-rare unowned accepts") && ok;
    ok = expect(CanLotRareItem(false, true), "residual non-rare already-has accepts") && ok;
    ok = expect(CanLotRareItem(true, false), "residual rare unowned accepts") && ok;
    ok = expect(!CanLotRareItem(true, true), "residual rare already-has rejects") && ok;

    // --- Composition table: free == inline == pin (positive form only) ---
    const struct
    {
        bool        itemIsRare;
        bool        alreadyHasItem;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 1367 / 2998 poles.
        { false, false, true, "residual non-rare unowned accepts" },
        { false, true, true, "residual non-rare already-has accepts" },
        { true, false, true, "residual rare unowned accepts" },
        { true, true, false, "residual rare already-has rejects" },

        // Classic dual poles (dense 2²).
        { false, false, true, "non-rare unowned accepts" },
        { false, true, true, "non-rare already-has accepts" },
        { true, false, true, "rare unowned accepts" },
        { true, true, false, "rare already-has rejects" },

        // Residual 2772 preflight-facing pins.
        { true, false, true, "residual rare unowned preflight pin" },
        { true, true, false, "residual rare owned preflight pin" },

        // Prior dedicated 3260 poles still hold.
        { true, true, false, "prior 3260 rare owned reject" },
        { true, false, true, "prior 3260 rare unowned accept" },
        { false, true, true, "prior 3260 non-rare has accept" },
        { false, false, true, "prior 3260 non-rare unowned accept" },

        // Boundary re-pins of the reject cell and accept neighbors.
        { true, true, false, "rare owned boundary reject" },
        { true, false, true, "rare unowned boundary accept" },
        { false, true, true, "non-rare has boundary accept" },
        { false, false, true, "non-rare unowned boundary accept" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanLotRareItem(c.itemIsRare, c.alreadyHasItem);
        const bool inlineF = inlineCanLotRareItem(c.itemIsRare, c.alreadyHasItem);
        const bool pin     = pinCanLotRareItem(c.itemIsRare, c.alreadyHasItem);
        // Positive form only (avoid !(a&&b) De Morgan pin).
        const bool wantPin = !c.itemIsRare || !c.alreadyHasItem;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula !rare || !has") && ok;
    }

    // Explicit residual poles free == inline == pin for dense 2² cells.
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool got     = CanLotRareItem(rare, has);
            const bool inlineF = inlineCanLotRareItem(rare, has);
            const bool pin     = pinCanLotRareItem(rare, has);
            const bool want    = !rare || !has;
            ok                 = expect(got == want, "pole free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "pole free == inline == pin") && ok;
        }
    }

    // Dense 2² compose over both bool domains — free == inline == pin.
    // Positive form only: want = !rare || !has.
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool got     = CanLotRareItem(rare, has);
            const bool inlineF = inlineCanLotRareItem(rare, has);
            const bool pin     = pinCanLotRareItem(rare, has);
            const bool want    = !rare || !has;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host-style inject poles through PlanLotItemPreflight (all earlier gates
    // clear so rare-owned is the discriminating factor; freeSlots=1).
    // free == inline == pin and disposition polarity.
    ok = expect(!CanLotRareItem(true, true), "inject rare owned free rejects") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, true, true) == LotItemPreflight::RejectRareOwned,
                "inject rare owned → RejectRareOwned") &&
         ok;
    ok = expect(CanLotRareItem(true, false), "inject rare unowned free accepts") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, true, false) == LotItemPreflight::Proceed,
                "inject rare unowned → Proceed") &&
         ok;
    ok = expect(CanLotRareItem(false, true), "inject non-rare has free accepts") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, true) == LotItemPreflight::Proceed,
                "inject non-rare has → Proceed") &&
         ok;
    ok = expect(CanLotRareItem(false, false), "inject non-rare unowned free accepts") && ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "inject non-rare unowned → Proceed") &&
         ok;

    // Dual-wire: free gate polarity matches preflight disposition for rare gate
    // free == inline == pin across dense 2² (positive form).
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool canLot    = CanLotRareItem(rare, has);
            const bool inlineF   = inlineCanLotRareItem(rare, has);
            const bool pin       = pinCanLotRareItem(rare, has);
            const auto preflight = PlanLotItemPreflight(false, false, false, false, 1, rare, has);
            ok                   = expect(canLot == inlineF && canLot == pin, "preflight free == inline == pin") && ok;
            if (canLot)
            {
                ok = expect(preflight == LotItemPreflight::Proceed, "canLot preflight Proceed") && ok;
            }
            else
            {
                ok = expect(preflight == LotItemPreflight::RejectRareOwned, "rare owned preflight Reject") && ok;
            }
        }
    }

    // Production lotItem path semantics:
    // rare owned → RejectRareOwned
    // rare unowned / non-rare → Proceed past rare gate
    ok = expect(!CanLotRareItem(true, true), "lotItem rare owned → reject path") && ok;
    ok = expect(CanLotRareItem(true, false), "lotItem rare unowned → accept path") && ok;
    ok = expect(CanLotRareItem(false, true), "lotItem non-rare has → accept path") && ok;
    ok = expect(CanLotRareItem(false, false), "lotItem non-rare unowned → accept path") && ok;

    return ok;
}
