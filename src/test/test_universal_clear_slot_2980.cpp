#include "test_universal_clear_slot_2980.h"

#include "map/universal_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal ShouldClearSlot 2980 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ClearSlot range gate for dual-wire cross-check (slice 2980):
//   slotInRange
auto inlineShouldClearSlot(const bool slotInRange) -> bool
{
    return slotInRange;
}

} // namespace

// Pure dual-wire expansion for ucontainerhelpers::ShouldClearSlot
// (ClearSlot range gate; slice 2980). Prior pure port: slice 2813.
auto runUniversalClearSlot2980SelfTests() -> bool
{
    using ucontainerhelpers::ShouldClearSlot;
    using ucontainerhelpers::ShouldAdjustCountOnClearSlot;

    bool ok = true;

    const struct
    {
        bool        slotInRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "in-range accepts" },
        { false, false, "out-of-range rejects" },

        // Residual 2813 pins (full 2^1 surface).
        { true, true, "residual in-range" },
        { false, false, "residual out-of-range" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearSlot(c.slotInRange);
        const bool inlineF = inlineShouldClearSlot(c.slotInRange);
        const bool wantPin = c.slotInRange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearSlot dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearSlot == pin formula slotInRange") && ok;
    }

    // Pin composition: identity of the range flag.
    ok = expect(ShouldClearSlot(true), "in-range must accept") && ok;
    ok = expect(!ShouldClearSlot(false), "out-of-range must reject") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool slotInRange : { false, true })
    {
        const bool got  = ShouldClearSlot(slotInRange);
        const bool want = slotInRange;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearSlot(slotInRange),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CUContainer::ClearSlot path semantics ---
    // Host injects:
    //   slotInRange = slotID < m_PItem.size()
    // when true  → m_PItem[slotID] = nullptr (count unchanged)
    // when false → leave state
    // Unlike SetItem, ClearSlot does NOT inject m_lock.
    ok = expect(ShouldClearSlot(true), "in-range inject → clear path") && ok;
    ok = expect(!ShouldClearSlot(false), "out-of-range inject → no-op path") && ok;

    // Host-style inject poles (mirrors universal_container.cpp probes).
    for (const bool slotInRange : { false, true })
    {
        const bool inject = slotInRange;
        ok                = expect(ShouldClearSlot(slotInRange) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldClearSlot(slotInRange) == inlineShouldClearSlot(slotInRange),
                    "host inject free == inline") &&
             ok;
    }

    // Residual 2813: ClearSlot never adjusts m_count.
    ok = expect(!ShouldAdjustCountOnClearSlot(), "ClearSlot never adjusts count") && ok;

    return ok;
}
