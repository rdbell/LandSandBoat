#include "test_trade_set_confirmed_2962.h"

#include "map/trade_container_capacity.h"

#include <iostream>
#include <tuple>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "trade ShouldAllowSetConfirmedStatus 2962 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline setConfirmedStatus outer gate for dual-wire cross-check (slice 2962):
//   slotInRange && itemNonNull && quantityGteAmount
auto inlineShouldAllowSetConfirmedStatus(
    const bool slotInRange,
    const bool itemNonNull,
    const bool quantityGteAmount) -> bool
{
    return slotInRange && itemNonNull && quantityGteAmount;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldAllowSetConfirmedStatus
// (setConfirmedStatus outer gate; slice 2962).
auto runTradeSetConfirmed2962SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldAllowSetConfirmedStatus;

    bool ok = true;

    const struct
    {
        bool        slotInRange;
        bool        itemNonNull;
        bool        quantityGteAmount;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, true, true, "all true accepts" },
        { false, true, true, false, "out of range rejects" },
        { true, false, true, false, "null item rejects" },
        { true, true, false, false, "quantity low rejects" },

        // Residual 2806 pins (full 2^3 surface).
        { false, false, false, false, "residual all false" },
        { false, false, true, false, "residual range+null" },
        { true, false, false, false, "residual null+qty" },
        { false, true, false, false, "residual range+qty" },
        { true, true, true, true, "residual all true" },
        { false, true, true, false, "residual out of range" },
        { true, false, true, false, "residual null item" },
        { true, true, false, false, "residual quantity low" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowSetConfirmedStatus(c.slotInRange, c.itemNonNull, c.quantityGteAmount);
        const bool inlineF = inlineShouldAllowSetConfirmedStatus(c.slotInRange, c.itemNonNull, c.quantityGteAmount);
        const bool wantPin = c.slotInRange && c.itemNonNull && c.quantityGteAmount;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAllowSetConfirmedStatus dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAllowSetConfirmedStatus == pin formula AND of three") && ok;
    }

    // Pin composition: only the all-true triple admits.
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "out of range must reject") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, true), "null item must reject") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "quantity low must reject") && ok;

    // Dense compose: full 2^3 boolean space.
    for (const bool slotInRange : { false, true })
    {
        for (const bool itemNonNull : { false, true })
        {
            for (const bool quantityGteAmount : { false, true })
            {
                const bool got  = ShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
                const bool want = slotInRange && itemNonNull && quantityGteAmount;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // --- Production CTradeContainer::setConfirmedStatus path semantics ---
    // Host injects:
    //   slotInRange       = slotID < m_PItem.size()
    //   itemNonNull       = slotInRange && m_PItem[slotID] != nullptr
    //   quantityGteAmount = itemNonNull && getQuantity() >= amount
    // when true  → write ConfirmedStatusAmount; return true
    // when false → leave state; return false
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "in-range non-null qty-ok → accept path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, false), "out-of-range inject → reject path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, false), "null item inject → reject path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "quantity low inject → reject path") && ok;

    // Host-style inject poles (mirrors trade_container.cpp probes).
    for (const auto& pole : {
             // slotInRange, itemNonNull, quantityGteAmount
             std::tuple<bool, bool, bool>{ false, false, false }, // out of range
             std::tuple<bool, bool, bool>{ true, false, false },  // null item
             std::tuple<bool, bool, bool>{ true, true, false },   // qty short
             std::tuple<bool, bool, bool>{ true, true, true },    // admit
         })
    {
        const bool slotInRange       = std::get<0>(pole);
        const bool itemNonNull       = std::get<1>(pole);
        const bool quantityGteAmount = std::get<2>(pole);
        const bool inject            = slotInRange && itemNonNull && quantityGteAmount;
        ok                           = expect(ShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount) ==
                        inlineShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
