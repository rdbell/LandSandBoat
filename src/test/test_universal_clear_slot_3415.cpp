#include "test_universal_clear_slot_3415.h"

#include "map/universal_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal ShouldClearSlot 3415 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ClearSlot range gate for dual-wire cross-check (dedicated 3415):
//   slotInRange
auto inlineShouldClearSlot(const bool slotInRange) -> bool
{
    return slotInRange;
}

// Compact dual-wire pin matching Go pinShouldClearSlot3415 / C++ capacity:
//   slotInRange
auto pinShouldClearSlot(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for ucontainerhelpers::ShouldClearSlot
// (ClearSlot range gate; OmegaXI internal/universalcontainer;
// dedicated slice 3415 expand residual 2813 / prior 2980 / prior 3354).
//
// Coverage:
//   - free == inline == pin == slotInRange
//   - residual poles (2813 / prior 2980 / prior 3354) still hold
//   - dense 2^1 free == inline == pin
auto runUniversalClearSlot3415SelfTests() -> bool
{
    using ucontainerhelpers::ShouldClearSlot;
    using ucontainerhelpers::ShouldAdjustCountOnClearSlot;

    bool ok = true;

    // Residual 2813 / prior 2980 / prior 3354 pins still hold under dual-wire.
    ok = expect(ShouldClearSlot(true), "residual: in-range accepts") && ok;
    ok = expect(!ShouldClearSlot(false), "residual: out-of-range rejects") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 2813+prior 2980+prior 3354 surface.
        { true, true, "in-range accepts" },
        { false, false, "out-of-range rejects" },

        // Residual 2813 / prior 2980 / prior 3354 pins (full 2^1 surface, labeled).
        { true, true, "residual in-range" },
        { false, false, "residual out-of-range" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearSlot(c.slotInRange);
        const bool inlineF = inlineShouldClearSlot(c.slotInRange);
        const bool pin     = pinShouldClearSlot(c.slotInRange);
        // Pin form: slotInRange (identity of the range flag).
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        // free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula slotInRange") && ok;
    }

    // Direct return pin: identity of the range flag.
    ok = expect(ShouldClearSlot(true), "in-range must accept") && ok;
    ok = expect(!ShouldClearSlot(false), "out-of-range must reject") && ok;
    // inline+pin direct return identity on accept/reject poles.
    ok = expect(inlineShouldClearSlot(true) && pinShouldClearSlot(true),
                "inline/pin direct return must accept in-range") &&
         ok;
    ok = expect(!inlineShouldClearSlot(false) && !pinShouldClearSlot(false),
                "inline/pin direct return must reject out-of-range") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldClearSlot(slotInRange);
        const bool inlineF = inlineShouldClearSlot(slotInRange);
        const bool pin     = pinShouldClearSlot(slotInRange);
        const bool want    = slotInRange;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // Host-style inject poles: free == inline == pin for ClearSlot.
    for (const bool slotInRange : { false, true })
    {
        const bool got     = ShouldClearSlot(slotInRange);
        const bool inlineF = inlineShouldClearSlot(slotInRange);
        const bool pin     = pinShouldClearSlot(slotInRange);
        const bool inject  = slotInRange;
        ok                 = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production ClearSlot path semantics:
    //   slotInRange = slotID < m_PItem.size()
    // when true  → m_PItem[slotID] = nullptr (count unchanged)
    // when false → leave state
    // Unlike SetItem, ClearSlot does NOT inject m_lock.
    ok = expect(ShouldClearSlot(true), "in-range inject → clear path") && ok;
    ok = expect(!ShouldClearSlot(false), "out-of-range inject → no-op path") && ok;

    // Residual 2813: ClearSlot never adjusts m_count.
    ok = expect(!ShouldAdjustCountOnClearSlot(), "ClearSlot never adjusts count") && ok;

    return ok;
}
