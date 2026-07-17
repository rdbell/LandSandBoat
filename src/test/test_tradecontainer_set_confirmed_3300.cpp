#include "test_tradecontainer_set_confirmed_3300.h"

#include "map/trade_container_capacity.h"

#include <iostream>
#include <tuple>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "tradecontainer ShouldAllowSetConfirmedStatus 3300 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline setConfirmedStatus outer gate for dual-wire cross-check (dedicated 3300):
//   slotInRange && itemNonNull && quantityGteAmount
auto inlineShouldAllowSetConfirmedStatus(
    const bool slotInRange,
    const bool itemNonNull,
    const bool quantityGteAmount) -> bool
{
    return slotInRange && itemNonNull && quantityGteAmount;
}

// Compact dual-wire pin matching Go pinShouldAllowSetConfirmedStatus3300 / C++ capacity:
//   slotInRange && itemNonNull && quantityGteAmount
auto pinShouldAllowSetConfirmedStatus(
    const bool slotInRange,
    const bool itemNonNull,
    const bool quantityGteAmount) -> bool
{
    return slotInRange && itemNonNull && quantityGteAmount;
}

} // namespace

// Pure dual-wire expansion for tradecontainerhelpers::ShouldAllowSetConfirmedStatus
// (setConfirmedStatus outer gate; OmegaXI internal/tradecontainer;
// dedicated slice 3300 expand residual 2962; prior dedicated 3175 / 3268).
//
// Coverage:
//   - free == inline == pin == (slotInRange && itemNonNull && quantityGteAmount)
//   - residual 2806 / 2962 / 3175 / 3268 pins still hold
//   - dense 2^3 boolean space
auto runTradecontainerSetConfirmed3300SelfTests() -> bool
{
    using tradecontainerhelpers::ShouldAllowSetConfirmedStatus;

    bool ok = true;

    // Residual 2806 / 2962 / 3175 / 3268 pins still hold under dual-wire.
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "residual all true accepts") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "residual out of range rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, true), "residual null item rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "residual quantity low rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, false), "residual all false rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, false), "residual null+qty rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, false), "residual range+qty rejects") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, true), "residual range+null rejects") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        slotInRange;
        bool        itemNonNull;
        bool        quantityGteAmount;
        bool        want;
        const char* label;
    } cases[] = {
        // Residual 2962 / 2806 / 3175 / 3268 poles (full 2^3 surface).
        { true, true, true, true, "residual all true" },
        { false, true, true, false, "residual out of range" },
        { true, false, true, false, "residual null item" },
        { true, true, false, false, "residual quantity low" },
        { false, false, false, false, "residual all false" },
        { false, false, true, false, "residual range+null" },
        { true, false, false, false, "residual null+qty" },
        { false, true, false, false, "residual range+qty" },

        // Classic dual poles.
        { true, true, true, true, "all true accepts" },
        { false, true, true, false, "out of range rejects" },
        { true, false, true, false, "null item rejects" },
        { true, true, false, false, "quantity low rejects" },

        // Host-style inject poles.
        { false, false, false, false, "host out-of-range inject" },
        { true, false, false, false, "host null-item inject" },
        { true, true, false, false, "host qty-short inject" },
        { true, true, true, true, "host admit inject" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowSetConfirmedStatus(c.slotInRange, c.itemNonNull, c.quantityGteAmount);
        const bool inlineF = inlineShouldAllowSetConfirmedStatus(c.slotInRange, c.itemNonNull, c.quantityGteAmount);
        const bool pin     = pinShouldAllowSetConfirmedStatus(c.slotInRange, c.itemNonNull, c.quantityGteAmount);
        const bool wantPin = c.slotInRange && c.itemNonNull && c.quantityGteAmount;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula AND of three") && ok;
    }

    // Pin composition: only the all-true triple admits.
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "all true must accept") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, true, true), "out of range must reject") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, true), "null item must reject") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "quantity low must reject") && ok;

    // Dense compose: full 2^3 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        for (const bool itemNonNull : { false, true })
        {
            for (const bool quantityGteAmount : { false, true })
            {
                const bool got     = ShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
                const bool inlineF = inlineShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
                const bool pin     = pinShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
                const bool want    = slotInRange && itemNonNull && quantityGteAmount;
                ok                 = expect(got == want, "compose free == pin formula") && ok;
                ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
            }
        }
    }

    // Host-style inject poles: free == inline == pin for setConfirmedStatus.
    for (const auto& pole : {
             // slotInRange, itemNonNull, quantityGteAmount
             std::tuple<bool, bool, bool>{ false, false, false }, // out of range
             std::tuple<bool, bool, bool>{ true, false, false },  // null item
             std::tuple<bool, bool, bool>{ true, true, false },   // qty short
             std::tuple<bool, bool, bool>{ true, true, true },    // admit
             std::tuple<bool, bool, bool>{ false, true, true },
             std::tuple<bool, bool, bool>{ true, false, true },
             std::tuple<bool, bool, bool>{ false, false, true },
             std::tuple<bool, bool, bool>{ false, true, false },
         })
    {
        const bool slotInRange       = std::get<0>(pole);
        const bool itemNonNull       = std::get<1>(pole);
        const bool quantityGteAmount = std::get<2>(pole);
        const bool got               = ShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
        const bool inlineF           = inlineShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
        const bool pin               = pinShouldAllowSetConfirmedStatus(slotInRange, itemNonNull, quantityGteAmount);
        const bool inject            = slotInRange && itemNonNull && quantityGteAmount;
        ok                           = expect(got == inject, "host inject dual-wire identity") && ok;
        ok = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production setConfirmedStatus path semantics:
    //   range && non-null && qty-ok → write ConfirmedStatusAmount; return true
    //   else → leave state; return false
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, true), "in-range non-null qty-ok → accept path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(false, false, false), "out-of-range inject → reject path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, false, false), "null item inject → reject path") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, false), "quantity low inject → reject path") && ok;

    // Host-style quantity comparisons (10>=4 admit / 10>=11 reject).
    ok = expect(ShouldAllowSetConfirmedStatus(true, true, 10 >= 4), "qty-ok inject must accept") && ok;
    ok = expect(!ShouldAllowSetConfirmedStatus(true, true, 10 >= 11), "over-qty inject must reject") && ok;

    return ok;
}
