#include "test_treasure_reject_null_item_3067.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldRejectNullItem 3067 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem null-item formula for dual-wire cross-check (slice 3067):
//   itemNull
auto inlineShouldRejectNullItem(const bool itemNull) -> bool
{
    return itemNull;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldRejectNullItem
// (itemNull identity null-item gate; slice 3067). Dense 2¹.
auto runTreasureRejectNullItem3067SelfTests() -> bool
{
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;
    using treasurepoolhelpers::ShouldRejectNullItem;
    using treasurepoolhelpers::ShouldRejectNullMember;

    bool ok = true;

    const struct
    {
        bool        itemNull;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles (both values of one bool).
        { false, false, "non-null item proceeds" },
        { true, true, "null item rejects" },

        // Residual 1367 / 2772 pins.
        { true, true, "residual null item" },
        { false, false, "residual accept item" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullItem(c.itemNull);
        const bool inlineF = inlineShouldRejectNullItem(c.itemNull);
        const bool wantPin = c.itemNull;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullItem dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNullItem == pin formula itemNull") && ok;
    }

    // Pin composition: reject only when itemNull is true.
    ok = expect(!ShouldRejectNullItem(false), "non-null item must proceed") && ok;
    ok = expect(ShouldRejectNullItem(true), "null item must reject") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool itemNull : { false, true })
    {
        const bool got  = ShouldRejectNullItem(itemNull);
        const bool want = itemNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullItem(itemNull), "compose free == inline") && ok;
    }

    // Explicit dual-wire identity: free == inline == pin identity.
    for (const bool itemNull : { false, true })
    {
        const bool freeF   = ShouldRejectNullItem(itemNull);
        const bool inlineF = inlineShouldRejectNullItem(itemNull);
        const bool pin     = itemNull;
        ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(ShouldRejectNullItem(true), "residual null item pin") && ok;
    ok = expect(!ShouldRejectNullItem(false), "residual accept item pin") && ok;

    // Sibling dual-wire independence: ShouldRejectNullMember remains distinct.
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true),
                "sibling ShouldRejectNullMember dual-wire polarity") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false), "sibling valid member proceeds") && ok;
    ok = expect(!ShouldRejectNullItem(false) && !ShouldRejectNullMember(false, false),
                "non-null item + valid member both proceed") &&
         ok;
    ok = expect(ShouldRejectNullItem(true), "null item rejects even when member valid") && ok;
    ok = expect(ShouldRejectNullMember(true, false), "null member rejects even when item non-null") && ok;

    // Host-style inject poles through PlanLotItemPreflight (member + slot clear).
    ok = expect(PlanLotItemPreflight(false, false, false, true, 1, false, false) == LotItemPreflight::RejectItem,
                "lotItem null item → RejectItem") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "lotItem non-null item → Proceed") &&
         ok;
    ok = expect(PlanLotItemPreflight(true, false, false, true, 0, true, true) == LotItemPreflight::RejectMember,
                "lotItem order member first") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, true, true, 0, true, true) == LotItemPreflight::RejectSlot,
                "lotItem order slot before item") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, true, 0, true, true) == LotItemPreflight::RejectItem,
                "lotItem order item before inv") &&
         ok;

    // Dual-wire: free gate polarity matches preflight disposition.
    for (const bool itemNull : { false, true })
    {
        const bool reject = ShouldRejectNullItem(itemNull);
        ok                = expect(reject == inlineShouldRejectNullItem(itemNull), "preflight free == inline") && ok;
        ok                = expect(reject == itemNull, "preflight free == pin identity") && ok;

        const auto lot = PlanLotItemPreflight(false, false, false, itemNull, 1, false, false);

        if (reject)
        {
            ok = expect(lot == LotItemPreflight::RejectItem, "reject lot RejectItem") && ok;
        }
        else
        {
            ok = expect(lot == LotItemPreflight::Proceed, "accept lot Proceed") && ok;
        }
    }

    return ok;
}
