#include "test_treasure_reject_null_member_3060.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldRejectNullMember 3060 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline lotItem / passItem / UpdatePool null-member formula for dual-wire
// cross-check (slice 3060):
//   charNull || poolMismatch
auto inlineShouldRejectNullMember(const bool charNull, const bool poolMismatch) -> bool
{
    return charNull || poolMismatch;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldRejectNullMember
// (charNull || poolMismatch null-member gate; slice 3060). Dense 2².
auto runTreasureRejectNullMember3060SelfTests() -> bool
{
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PassItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;
    using treasurepoolhelpers::PlanPassItemPreflight;
    using treasurepoolhelpers::PlanUpdatePool;
    using treasurepoolhelpers::ShouldRejectNullItem;
    using treasurepoolhelpers::ShouldRejectNullMember;

    bool ok = true;

    const struct
    {
        bool        charNull;
        bool        poolMismatch;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2² dual poles (all combinations of two bools).
        { false, false, false, "valid member proceeds" },
        { false, true, true, "pool mismatch rejects" },
        { true, false, true, "null char rejects" },
        { true, true, true, "null char and mismatch rejects" },

        // Residual 1367 / 2772 / 2777 / 2780 pins.
        { true, false, true, "residual null char" },
        { false, true, true, "residual pool mismatch" },
        { false, false, false, "residual accept member" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullMember(c.charNull, c.poolMismatch);
        const bool inlineF = inlineShouldRejectNullMember(c.charNull, c.poolMismatch);
        const bool wantPin = c.charNull || c.poolMismatch;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectNullMember == pin formula charNull || poolMismatch") && ok;
    }

    // Pin composition: reject when either flag is true.
    ok = expect(!ShouldRejectNullMember(false, false), "valid member must proceed") && ok;
    ok = expect(ShouldRejectNullMember(true, false), "null char must reject") && ok;
    ok = expect(ShouldRejectNullMember(false, true), "pool mismatch must reject") && ok;
    ok = expect(ShouldRejectNullMember(true, true), "null char and mismatch must reject") && ok;

    // Dense 2² compose over both bool domains (exactly four cells).
    for (const bool charNull : { false, true })
    {
        for (const bool poolMismatch : { false, true })
        {
            const bool got  = ShouldRejectNullMember(charNull, poolMismatch);
            const bool want = charNull || poolMismatch;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldRejectNullMember(charNull, poolMismatch),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Explicit dual-wire identity: free == inline == pin OR.
    for (const bool charNull : { false, true })
    {
        for (const bool poolMismatch : { false, true })
        {
            const bool freeF   = ShouldRejectNullMember(charNull, poolMismatch);
            const bool inlineF = inlineShouldRejectNullMember(charNull, poolMismatch);
            const bool pin     = charNull || poolMismatch;
            ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
        }
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true),
                "residual null member pins") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false), "residual accept member pin") && ok;

    // Sibling residual independence: ShouldRejectNullItem is not dual-wired
    // in 3060 and remains distinct (itemNull identity).
    ok = expect(ShouldRejectNullItem(true) && !ShouldRejectNullItem(false),
                "sibling ShouldRejectNullItem residual identity") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false) && !ShouldRejectNullItem(false),
                "valid member + non-null item both proceed") &&
         ok;
    ok = expect(ShouldRejectNullMember(true, false), "null member rejects even when item non-null") && ok;
    ok = expect(ShouldRejectNullItem(true), "null item rejects even when member valid") && ok;

    // Host-style inject poles through PlanLotItemPreflight (later gates clear).
    ok = expect(PlanLotItemPreflight(true, false, false, false, 1, false, false) == LotItemPreflight::RejectMember,
                "lotItem null char → RejectMember") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, true, false, false, 1, false, false) == LotItemPreflight::RejectMember,
                "lotItem pool mismatch → RejectMember") &&
         ok;
    ok = expect(PlanLotItemPreflight(false, false, false, false, 1, false, false) == LotItemPreflight::Proceed,
                "lotItem valid member → Proceed") &&
         ok;
    ok = expect(PlanLotItemPreflight(true, true, true, true, 0, true, true) == LotItemPreflight::RejectMember,
                "lotItem order member first") &&
         ok;

    // Host-style inject through PlanPassItemPreflight.
    ok = expect(PlanPassItemPreflight(true, false, false) == PassItemPreflight::RejectMember,
                "passItem null char → RejectMember") &&
         ok;
    ok = expect(PlanPassItemPreflight(false, true, false) == PassItemPreflight::RejectMember,
                "passItem pool mismatch → RejectMember") &&
         ok;
    ok = expect(PlanPassItemPreflight(false, false, false) == PassItemPreflight::Proceed,
                "passItem valid member → Proceed") &&
         ok;

    // Host-style inject through PlanUpdatePool.
    {
        const auto nullChar = PlanUpdatePool(true, false, false);
        ok                  = expect(nullChar.reject && !nullChar.pushTrophyLists, "update null char → Reject") && ok;
        const auto mismatch = PlanUpdatePool(false, true, false);
        ok                  = expect(mismatch.reject && !mismatch.pushTrophyLists, "update pool mismatch → Reject") && ok;
        const auto valid    = PlanUpdatePool(false, false, false);
        ok                  = expect(!valid.reject && valid.pushTrophyLists, "update valid visible → PushTrophyLists") && ok;
    }

    // Dual-wire: free gate polarity matches preflight / update disposition.
    for (const bool charNull : { false, true })
    {
        for (const bool poolMismatch : { false, true })
        {
            const bool reject = ShouldRejectNullMember(charNull, poolMismatch);
            ok                = expect(reject == inlineShouldRejectNullMember(charNull, poolMismatch),
                        "preflight free == inline") &&
                 ok;
            ok = expect(reject == (charNull || poolMismatch), "preflight free == pin OR") && ok;

            const auto lot    = PlanLotItemPreflight(charNull, poolMismatch, false, false, 1, false, false);
            const auto pass   = PlanPassItemPreflight(charNull, poolMismatch, false);
            const auto update = PlanUpdatePool(charNull, poolMismatch, false);

            if (reject)
            {
                ok = expect(lot == LotItemPreflight::RejectMember, "reject lot RejectMember") && ok;
                ok = expect(pass == PassItemPreflight::RejectMember, "reject pass RejectMember") && ok;
                ok = expect(update.reject && !update.pushTrophyLists, "reject update Reject") && ok;
            }
            else
            {
                ok = expect(lot == LotItemPreflight::Proceed, "accept lot Proceed") && ok;
                ok = expect(pass == PassItemPreflight::Proceed, "accept pass Proceed") && ok;
                ok = expect(!update.reject && update.pushTrophyLists, "accept update PushTrophyLists") && ok;
            }
        }
    }

    return ok;
}
