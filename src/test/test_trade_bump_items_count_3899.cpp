#include "test_trade_bump_items_count_3899.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade ShouldBumpItemsCountOnSetEntry 3899 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline multi-arg setItem ItemsCount bump gate for dual-wire cross-check
// (dedicated 3899):
//   slotInRange
auto inlineShouldBumpItemsCountOnSetEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// Compact dual-wire pin matching Go pinShouldBumpItemsCountOnSetEntry3899 / C++ capacity:
//   slotInRange
auto pinShouldBumpItemsCountOnSetEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// Prior dedicated 3854 inline pin for independence cross-check.
auto inlineShouldBumpItemsCountOnSetEntry3854(const bool slotInRange) -> bool
{
    return slotInRange;
}

// Prior dedicated 3854 compact pin for free == inline == pin == pin3854 cross-check.
auto pinShouldBumpItemsCountOnSetEntry3854(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry
// (multi-arg setItem ItemsCount bump gate; OmegaXI internal/tradecontainer;
// dedicated slice 3899 expand residual 2997; prior dedicated 3854 / 3809 / 3764 / 3719 / 3674 / 3629 / 3584 / 3540 / 3474 / 3420 / 3372).
//
// Coverage:
//   - free == inline == pin == pin3854 == slotInRange (identity)
//   - residual 2812 / 2997 / prior dedicated 3372 / 3420 / 3474 / 3540 / 3584 / 3629 / 3674 / 3719 / 3764 / 3809 / 3854 pins still hold
//   - prior dedicated 3854 independence (free == prior inline == prior pin)
//   - dense 2^1 boolean space
// Sibling dual-wires left residual (not re-expanded / not thrashed):
//   ShouldSetTradeItemEntry (3211), ShouldAllowSetConfirmedStatus (3300).
// Formula unchanged — not registered in CMake/main.
auto runTradeBumpItemsCount3899SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldAllowSetConfirmedStatus;
    using tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry;
    using tradecontainerhelpers::ShouldSetTradeItemEntry;

    bool ok = true;

    // Residual 2812 / 2997 / prior dedicated 3372 / 3420 / 3474 / 3540 / 3584 / 3629 / 3674 / 3719 / 3764 / 3809 / 3854 pins still hold under dual-wire.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "residual bump in range") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "residual no bump out of range") && ok;

    // --- Composition table: free == inline == pin == pin3854 ---
    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2997 / 2812 / prior 3372 / 3420 / 3474 / 3540 / 3584 / 3629 / 3674 / 3719 / 3764 / 3809 / 3854 poles (full 2^1 surface).
        { true, true, "residual bump" },
        { false, false, "residual no bump" },

        // Classic dual poles.
        { true, true, "in range bumps" },
        { false, false, "out of range no bump" },

        // Host-style inject poles.
        { false, false, "host out-of-range inject" },
        { true, true, "host bump inject" },

        // Prior dedicated 3854 poles still hold.
        { true, true, "prior 3854 in range bumps" },
        { false, false, "prior 3854 out of range no bump" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBumpItemsCountOnSetEntry(c.slotInRange);
        const bool inlineF = inlineShouldBumpItemsCountOnSetEntry(c.slotInRange);
        const bool pin     = pinShouldBumpItemsCountOnSetEntry(c.slotInRange);
        const bool pin3854 = pinShouldBumpItemsCountOnSetEntry3854(c.slotInRange);
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin == pin3854.
        ok = expect(got == inlineF && got == pin && got == pin3854,
                    "dual-wire free == inline == pin == pin3854") &&
             ok;
        ok = expect(got == wantPin, "free == pin formula identity") && ok;
    }

    // Free == pin == pin3854 across residual poles.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true) == pinShouldBumpItemsCountOnSetEntry(true),
                "free == pin for true") &&
         ok;
    ok = expect(ShouldBumpItemsCountOnSetEntry(false) == pinShouldBumpItemsCountOnSetEntry(false),
                "free == pin for false") &&
         ok;
    ok = expect(ShouldBumpItemsCountOnSetEntry(true) == pinShouldBumpItemsCountOnSetEntry3854(true),
                "free == pin3854 for true") &&
         ok;
    ok = expect(ShouldBumpItemsCountOnSetEntry(false) == pinShouldBumpItemsCountOnSetEntry3854(false),
                "free == pin3854 for false") &&
         ok;

    // Prior dedicated 3854 independence: free still matches prior inline/pin.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true) == inlineShouldBumpItemsCountOnSetEntry3854(true) &&
                    ShouldBumpItemsCountOnSetEntry(true) == pinShouldBumpItemsCountOnSetEntry3854(true),
                "prior 3854 free/inline/pin mismatch for true") &&
         ok;
    ok = expect(ShouldBumpItemsCountOnSetEntry(false) == inlineShouldBumpItemsCountOnSetEntry3854(false) &&
                    ShouldBumpItemsCountOnSetEntry(false) == pinShouldBumpItemsCountOnSetEntry3854(false),
                "prior 3854 free/inline/pin mismatch for false") &&
         ok;

    // Pin composition: only true bumps.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "in range must bump") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "out of range must not bump") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin == pin3854.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool inlineF = inlineShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool pin     = pinShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool pin3854 = pinShouldBumpItemsCountOnSetEntry3854(slotInRange);
        const bool want    = slotInRange;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3854,
                                    "compose free == inline == pin == pin3854") &&
             ok;
        // Prior 3854 independence across dense poles.
        ok = expect(got == inlineShouldBumpItemsCountOnSetEntry3854(slotInRange) &&
                        got == pinShouldBumpItemsCountOnSetEntry3854(slotInRange),
                    "compose free == prior 3854 inline/pin") &&
             ok;
    }

    // Host-style inject poles: free == inline == pin == pin3854 for multi-arg setItem bump.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool inlineF = inlineShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool pin     = pinShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool pin3854 = pinShouldBumpItemsCountOnSetEntry3854(slotInRange);
        const bool inject  = slotInRange;
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin && got == pin3854,
                                    "host inject free == inline == pin == pin3854") &&
             ok;
    }

    // Production multi-arg setItem count path semantics:
    //   slotInRange → bump path (even replace / clear — parity quirk)
    //   !slotInRange → no bump (outer gate also rejects)
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "in-range host inject → bump path") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "out-of-range host inject → no-bump path") && ok;

    // Sibling dual-wires left residual under this slice (not re-expanded / not thrashed).
    ok = expect(ShouldSetTradeItemEntry(true), "sibling residual admit in range (3211)") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "sibling residual reject out of range (3211)") && ok;
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "sibling residual setConfirmed admit (3300)") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "sibling residual setConfirmed reject (3300)") && ok;

    return ok;
}
