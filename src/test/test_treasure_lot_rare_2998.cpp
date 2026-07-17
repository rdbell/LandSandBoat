#include "test_treasure_lot_rare_2998.h"

#include "map/treasure_pool_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure CanLotRareItem 2998 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem rare-owned formula for dual-wire cross-check (slice 2998):
//   !(itemIsRare && alreadyHasItem)
auto inlineCanLotRareItem(const bool itemIsRare, const bool alreadyHasItem) -> bool
{
    return !(itemIsRare && alreadyHasItem);
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::CanLotRareItem
// (!(rare && alreadyHas) rare-owned lot gate; slice 2998). Dense 2².
auto runTreasureLotRare2998SelfTests() -> bool
{
    using treasurepoolhelpers::CanLotRareItem;
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    bool ok = true;

    const struct
    {
        bool        itemIsRare;
        bool        alreadyHasItem;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2² dual poles (all combinations of two bools).
        { false, false, true, "non-rare unowned accepts" },
        { false, true, true, "non-rare already-has accepts" },
        { true, false, true, "rare unowned accepts" },
        { true, true, false, "rare already-has rejects" },

        // Residual 1367 / 2772 pins.
        { true, false, true, "residual rare unowned" },
        { true, true, false, "residual rare owned" },

        // Boundary re-pins of the reject cell and accept neighbors.
        { true, true, false, "rare owned boundary reject" },
        { true, false, true, "rare unowned boundary accept" },
        { false, true, true, "non-rare has boundary accept" },
        { false, false, true, "non-rare unowned boundary accept" },
    };

    for (const auto& c : cases)
    {
        const bool got        = CanLotRareItem(c.itemIsRare, c.alreadyHasItem);
        const bool inlineF    = inlineCanLotRareItem(c.itemIsRare, c.alreadyHasItem);
        const bool wantPin    = !(c.itemIsRare && c.alreadyHasItem);
        const bool wantDeMorg = !c.itemIsRare || !c.alreadyHasItem;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanLotRareItem dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanLotRareItem == pin formula !(rare && has)") && ok;
        ok = expect(got == wantDeMorg, "CanLotRareItem == De Morgan !rare || !has") && ok;
        ok = expect(wantPin == wantDeMorg, "pin and De Morgan agree") && ok;
    }

    // Pin composition: only rare && alreadyHas rejects.
    ok = expect(CanLotRareItem(false, false), "non-rare unowned must accept") && ok;
    ok = expect(CanLotRareItem(false, true), "non-rare already-has must accept") && ok;
    ok = expect(CanLotRareItem(true, false), "rare unowned must accept") && ok;
    ok = expect(!CanLotRareItem(true, true), "rare already-has must reject") && ok;

    // Dense 2² compose over both bool domains (exactly four cells).
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool got  = CanLotRareItem(rare, has);
            const bool want = !(rare && has);
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanLotRareItem(rare, has), "compose free == inline") && ok;
            ok              = expect(got == (!rare || !has), "compose free == De Morgan") && ok;
        }
    }

    // Explicit dual-wire identity: free == inline == pin == De Morgan.
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool freeF   = CanLotRareItem(rare, has);
            const bool inlineF = inlineCanLotRareItem(rare, has);
            const bool pin     = !(rare && has);
            const bool deMorg  = !rare || !has;
            ok                 = expect(freeF == inlineF && freeF == pin && freeF == deMorg,
                        "dual-wire free==inline==pin==DeMorgan") &&
                 ok;
        }
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(CanLotRareItem(true, false) && !CanLotRareItem(true, true), "residual lot rare pins") && ok;

    // Production path semantics (host inject model):
    // itemIsRare / alreadyHasItem from item flag + inventory ownership lookup
    // when true  → PlanLotItemPreflight continues past rare gate (Proceed)
    // when false → PlanLotItemPreflight returns RejectRareOwned
    ok = expect(!CanLotRareItem(true, true), "rare owned → reject path") && ok;
    ok = expect(CanLotRareItem(true, false), "rare unowned → accept path") && ok;
    ok = expect(CanLotRareItem(false, true), "non-rare has → accept path") && ok;
    ok = expect(CanLotRareItem(false, false), "non-rare unowned → accept path") && ok;

    // Host-style inject poles through PlanLotItemPreflight (all earlier gates
    // clear so rare-owned is the discriminating factor; freeSlots=1).
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, true, true) == LotItemPreflight::RejectRareOwned,
                "lotItem rare owned → RejectRareOwned") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, true, false) == LotItemPreflight::Proceed,
                "lotItem rare unowned → Proceed") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, true) == LotItemPreflight::Proceed,
                "lotItem non-rare has → Proceed") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "lotItem non-rare unowned → Proceed") &&
         ok;

    // Dual-wire: free gate polarity matches preflight disposition for rare gate.
    for (const bool rare : { false, true })
    {
        for (const bool has : { false, true })
        {
            const bool canLot    = CanLotRareItem(rare, has);
            const auto preflight = PlanLotItemPreflight(false, false, false, false, 1, rare, has);
            if (canLot)
            {
                ok = expect(preflight == LotItemPreflight::Proceed, "canLot preflight Proceed") && ok;
            }
            else
            {
                ok = expect(preflight == LotItemPreflight::RejectRareOwned, "rare owned preflight Reject") && ok;
            }
            ok = expect(canLot == inlineCanLotRareItem(rare, has), "preflight free == inline") && ok;
            ok = expect(canLot == (!(rare && has)), "preflight free == pin") && ok;
        }
    }

    return ok;
}
