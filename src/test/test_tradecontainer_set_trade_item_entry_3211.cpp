#include "test_tradecontainer_set_trade_item_entry_3211.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "tradecontainer ShouldSetTradeItemEntry 3211 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline multi-arg setItem outer gate for dual-wire cross-check (dedicated 3211):
//   slotInRange
auto inlineShouldSetTradeItemEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// Compact dual-wire pin matching Go pinShouldSetTradeItemEntry3211 / C++ capacity:
//   slotInRange
auto pinShouldSetTradeItemEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldSetTradeItemEntry
// (multi-arg setItem outer gate; OmegaXI internal/tradecontainer;
// dedicated slice 3211 expand residual 2984).
//
// Coverage:
//   - free == inline == pin == slotInRange (identity)
//   - residual 2812 / 2984 pins still hold
//   - dense 2^1 boolean space
// Sibling dual-wires left residual: ShouldAllowSetConfirmedStatus (3175),
// ShouldBumpItemsCountOnSetEntry (2997).
auto runTradecontainerSetTradeItemEntry3211SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldAllowSetConfirmedStatus;
    using tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry;
    using tradecontainerhelpers::ShouldSetTradeItemEntry;

    bool ok = true;

    // Residual 2812 / 2984 pins still hold under dual-wire.
    ok = expect(ShouldSetTradeItemEntry(true), "residual admit in range") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "residual reject out of range") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2984 / 2812 poles (full 2^1 surface).
        { true, true, "residual admit" },
        { false, false, "residual reject" },

        // Classic dual poles.
        { true, true, "in range accepts" },
        { false, false, "out of range rejects" },

        // Host-style inject poles.
        { false, false, "host out-of-range inject" },
        { true, true, "host admit inject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSetTradeItemEntry(c.slotInRange);
        const bool inlineF = inlineShouldSetTradeItemEntry(c.slotInRange);
        const bool pin     = pinShouldSetTradeItemEntry(c.slotInRange);
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula identity") && ok;
    }

    // Pin composition: only true admits.
    ok = expect(ShouldSetTradeItemEntry(true), "in range must accept") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "out of range must reject") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldSetTradeItemEntry(slotInRange);
        const bool inlineF = inlineShouldSetTradeItemEntry(slotInRange);
        const bool pin     = pinShouldSetTradeItemEntry(slotInRange);
        const bool want    = slotInRange;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Host-style inject poles: free == inline == pin for multi-arg setItem.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldSetTradeItemEntry(slotInRange);
        const bool inlineF = inlineShouldSetTradeItemEntry(slotInRange);
        const bool pin     = pinShouldSetTradeItemEntry(slotInRange);
        const bool inject  = slotInRange;
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production multi-arg setItem path semantics:
    //   slotInRange → admit write + bump path
    //   !slotInRange → leave state unchanged
    ok = expect(ShouldSetTradeItemEntry(true), "in-range host inject → accept path") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "out-of-range host inject → reject path") && ok;

    // Sibling dual-wires left residual under this slice (not re-expanded).
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "sibling residual admit implies bump (2997)") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "sibling residual reject implies no bump (2997)") && ok;
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "sibling residual setConfirmed admit (3175)") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "sibling residual setConfirmed reject (3175)") && ok;

    return ok;
}
