#include "test_treasure_update_pool_char_3112.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldUpdatePoolForChar 3112 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdatePool visibility formula for dual-wire cross-check (slice 3112):
//   !isDisappear
auto inlineShouldUpdatePoolForChar(const bool isDisappear) -> bool
{
    return !isDisappear;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldUpdatePoolForChar
// (!isDisappear UpdatePool visibility gate; slice 3112). Dense 2¹.
auto runTreasureUpdatePoolChar3112SelfTests() -> bool
{
    using treasurepoolhelpers::PlanUpdatePool;
    using treasurepoolhelpers::ShouldRejectNullItem;
    using treasurepoolhelpers::ShouldRejectNullMember;
    using treasurepoolhelpers::ShouldSkipRareCheck;
    using treasurepoolhelpers::ShouldUpdatePoolForChar;

    bool ok = true;

    const struct
    {
        bool        isDisappear;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2¹ dual poles (both values of one bool).
        { false, true, "visible char updates pool" },
        { true, false, "disappeared char skips update" },

        // Residual 1367 / 2777 pins.
        { false, true, "residual visible update" },
        { true, false, "residual disappear no update" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpdatePoolForChar(c.isDisappear);
        const bool inlineF = inlineShouldUpdatePoolForChar(c.isDisappear);
        const bool wantPin = !c.isDisappear;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldUpdatePoolForChar dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUpdatePoolForChar == pin formula !isDisappear") && ok;
    }

    // Pin composition: update only when not disappeared.
    ok = expect(ShouldUpdatePoolForChar(false), "visible char must update") && ok;
    ok = expect(!ShouldUpdatePoolForChar(true), "disappeared char must not update") && ok;

    // Dense 2¹ compose over the single bool domain (exactly two cells).
    for (const bool isDisappear : { false, true })
    {
        const bool got  = ShouldUpdatePoolForChar(isDisappear);
        const bool want = !isDisappear;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldUpdatePoolForChar(isDisappear), "compose free == inline") && ok;
    }

    // Explicit dual-wire identity: free == inline == pin formula.
    for (const bool isDisappear : { false, true })
    {
        const bool freeF   = ShouldUpdatePoolForChar(isDisappear);
        const bool inlineF = inlineShouldUpdatePoolForChar(isDisappear);
        const bool pin     = !isDisappear;
        ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(ShouldUpdatePoolForChar(false), "residual visible update pin") && ok;
    ok = expect(!ShouldUpdatePoolForChar(true), "residual disappear no-update pin") && ok;

    // Sibling dual-wire independence: 3060 / 3067 / 3094 remain distinct.
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true),
                "sibling ShouldRejectNullMember dual-wire polarity") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false), "sibling valid member proceeds") && ok;
    ok = expect(ShouldRejectNullItem(true) && !ShouldRejectNullItem(false),
                "sibling ShouldRejectNullItem dual-wire polarity") &&
         ok;
    ok = expect(ShouldSkipRareCheck(false, true) && !ShouldSkipRareCheck(true, true),
                "sibling ShouldSkipRareCheck dual-wire polarity") &&
         ok;

    // Host-style inject poles through PlanUpdatePool (member gates clear).
    {
        const auto visible = PlanUpdatePool(false, false, false);
        ok                 = expect(!visible.reject && visible.pushTrophyLists, "update visible → PushTrophyLists") && ok;
    }
    {
        const auto disappear = PlanUpdatePool(false, false, true);
        ok                   = expect(!disappear.reject && !disappear.pushTrophyLists, "update disappear → no push") && ok;
    }
    {
        const auto nullChar = PlanUpdatePool(true, false, false);
        ok                  = expect(nullChar.reject && !nullChar.pushTrophyLists, "update order member first null char") && ok;
    }
    {
        const auto mismatch = PlanUpdatePool(false, true, false);
        ok                  = expect(mismatch.reject && !mismatch.pushTrophyLists, "update order member first mismatch") && ok;
    }
    {
        const auto both = PlanUpdatePool(true, true, true);
        ok              = expect(both.reject && !both.pushTrophyLists, "update reject suppresses disappear path") && ok;
    }

    // Dual-wire: free gate polarity matches update disposition when member is valid.
    for (const bool isDisappear : { false, true })
    {
        const bool update = ShouldUpdatePoolForChar(isDisappear);
        ok                = expect(update == inlineShouldUpdatePoolForChar(isDisappear), "update free == inline") && ok;
        ok                = expect(update == !isDisappear, "update free == pin") && ok;

        const auto plan = PlanUpdatePool(false, false, isDisappear);
        ok              = expect(!plan.reject, "valid member must not reject") && ok;
        ok              = expect(plan.pushTrophyLists == update, "PushTrophyLists == free") && ok;
    }

    // When member rejects, PushTrophyLists stays false regardless of disappear.
    for (const bool isDisappear : { false, true })
    {
        for (const bool poolMismatch : { false, true })
        {
            const auto plan = PlanUpdatePool(true, poolMismatch, isDisappear);
            ok              = expect(plan.reject && !plan.pushTrophyLists, "null char rejects") && ok;
        }
        {
            const auto plan = PlanUpdatePool(false, true, isDisappear);
            ok              = expect(plan.reject && !plan.pushTrophyLists, "pool mismatch rejects") && ok;
        }
    }

    // Residual policy compose still wires helpers into host inputs.
    {
        const auto proceed = PlanUpdatePool(ShouldRejectNullMember(false, false), false, false);
        ok                 = expect(!proceed.reject && proceed.pushTrophyLists, "compose helpers proceed visible") && ok;
    }
    ok = expect(ShouldUpdatePoolForChar(false), "compose free visible") && ok;
    {
        const auto plan = PlanUpdatePool(false, false, false);
        ok              = expect(!plan.reject && plan.pushTrophyLists, "compose free visible inject") && ok;
    }
    ok = expect(!ShouldUpdatePoolForChar(true), "compose free disappear") && ok;
    {
        const auto plan = PlanUpdatePool(false, false, true);
        ok              = expect(!plan.reject && !plan.pushTrophyLists, "compose free disappear inject") && ok;
    }

    return ok;
}
