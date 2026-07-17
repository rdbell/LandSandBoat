#include "test_trade_bump_items_count_2997.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade ShouldBumpItemsCountOnSetEntry 2997 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline multi-arg setItem ItemsCount bump gate for dual-wire cross-check
// (slice 2997):
//   slotInRange
auto inlineShouldBumpItemsCountOnSetEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry
// (multi-arg setItem ItemsCount bump gate; slice 2997).
auto runTradeBumpItemsCount2997SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry;
    using tradecontainerhelpers::ShouldSetTradeItemEntry;

    bool ok = true;

    // Residual 2812 pins still hold under dual-wire.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "residual bump in range") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "residual no bump out of range") && ok;

    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "in range bumps" },
        { false, false, "out of range no bump" },

        // Residual 2812 pins (full 2^1 surface).
        { true, true, "residual bump" },
        { false, false, "residual no bump" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldBumpItemsCountOnSetEntry(c.slotInRange);
        const bool inlineF = inlineShouldBumpItemsCountOnSetEntry(c.slotInRange);
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldBumpItemsCountOnSetEntry dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldBumpItemsCountOnSetEntry == pin formula identity") && ok;
    }

    // Pin composition: only true bumps.
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "in range must bump") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "out of range must not bump") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        const bool got  = ShouldBumpItemsCountOnSetEntry(slotInRange);
        const bool want = slotInRange;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldBumpItemsCountOnSetEntry(slotInRange),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CTradeContainer::setItem multi-arg path semantics ---
    // Host injects:
    //   slotInRange = slotId < m_PItem.size()
    // when true  → m_ItemsCount += 1 (even replace / clear — parity quirk)
    // when false → no bump (outer gate also rejects)
    ok = expect(ShouldBumpItemsCountOnSetEntry(true), "in-range host inject → bump path") && ok;
    ok = expect(!ShouldBumpItemsCountOnSetEntry(false), "out-of-range host inject → no-bump path") && ok;

    // Host-style inject poles (mirrors trade_container.cpp probes).
    for (const bool slotInRange : { false, true })
    {
        ok = expect(ShouldBumpItemsCountOnSetEntry(slotInRange) == slotInRange, "host inject dual-wire identity") && ok;
        ok = expect(ShouldBumpItemsCountOnSetEntry(slotInRange) == inlineShouldBumpItemsCountOnSetEntry(slotInRange),
                    "host inject free == inline") &&
             ok;
    }

    // Sibling dual-wire ShouldSetTradeItemEntry (2984): admit ↔ bump pairing.
    ok = expect(ShouldSetTradeItemEntry(true) && ShouldBumpItemsCountOnSetEntry(true), "admit implies bump") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false) && !ShouldBumpItemsCountOnSetEntry(false),
                "reject implies no bump") &&
         ok;

    return ok;
}
