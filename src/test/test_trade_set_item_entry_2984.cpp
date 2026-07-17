#include "test_trade_set_item_entry_2984.h"

#include "map/trade_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade ShouldSetTradeItemEntry 2984 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline multi-arg setItem outer gate for dual-wire cross-check (slice 2984):
//   slotInRange
auto inlineShouldSetTradeItemEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldSetTradeItemEntry
// (multi-arg setItem outer gate; slice 2984).
auto runTradeSetItemEntry2984SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldBumpItemsCountOnSetEntry;
    using tradecontainerhelpers::ShouldSetTradeItemEntry;

    bool ok = true;

    // Residual 2812 pins still hold under dual-wire.
    ok = expect(ShouldSetTradeItemEntry(true), "residual admit in range") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "residual reject out of range") && ok;

    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "in range accepts" },
        { false, false, "out of range rejects" },

        // Residual 2812 pins (full 2^1 surface).
        { true, true, "residual admit" },
        { false, false, "residual reject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSetTradeItemEntry(c.slotInRange);
        const bool inlineF = inlineShouldSetTradeItemEntry(c.slotInRange);
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSetTradeItemEntry dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSetTradeItemEntry == pin formula identity") && ok;
    }

    // Pin composition: only true admits.
    ok = expect(ShouldSetTradeItemEntry(true), "in range must accept") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "out of range must reject") && ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        const bool got  = ShouldSetTradeItemEntry(slotInRange);
        const bool want = slotInRange;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSetTradeItemEntry(slotInRange),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CTradeContainer::setItem multi-arg path semantics ---
    // Host injects:
    //   slotInRange = slotId < m_PItem.size()
    // when true  → bump ItemsCount (ShouldBumpItemsCountOnSetEntry); assign slot
    // when false → leave state unchanged
    ok = expect(ShouldSetTradeItemEntry(true), "in-range host inject → accept path") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false), "out-of-range host inject → reject path") && ok;

    // Host-style inject poles (mirrors trade_container.cpp probes).
    for (const bool slotInRange : { false, true })
    {
        ok = expect(ShouldSetTradeItemEntry(slotInRange) == slotInRange, "host inject dual-wire identity") && ok;
        ok = expect(ShouldSetTradeItemEntry(slotInRange) == inlineShouldSetTradeItemEntry(slotInRange),
                    "host inject free == inline") &&
             ok;
    }

    // Residual 2812 bump pairing: admit implies bump; reject implies no bump.
    ok = expect(ShouldSetTradeItemEntry(true) && ShouldBumpItemsCountOnSetEntry(true), "admit implies bump") && ok;
    ok = expect(!ShouldSetTradeItemEntry(false) && !ShouldBumpItemsCountOnSetEntry(false),
                "reject implies no bump") &&
         ok;

    return ok;
}
