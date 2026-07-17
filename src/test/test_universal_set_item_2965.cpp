#include "test_universal_set_item_2965.h"

#include "map/universal_container_capacity.h"

#include <iostream>
#include <tuple>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universal ShouldAllowSetItem 2965 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SetItem outer gate for dual-wire cross-check (slice 2965):
//   slotInRange && !locked
auto inlineShouldAllowSetItem(const bool slotInRange, const bool locked) -> bool
{
    return slotInRange && !locked;
}

} // namespace

// Pure dual-wire expansion for ucontainerhelpers::ShouldAllowSetItem
// (SetItem outer gate; slice 2965).
auto runUniversalSetItem2965SelfTests() -> bool
{
    using ucontainerhelpers::ShouldAllowSetItem;

    bool ok = true;

    const struct
    {
        bool        slotInRange;
        bool        locked;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, false, true, "in-range unlocked accepts" },
        { true, true, false, "in-range locked rejects" },
        { false, false, false, "out-of-range unlocked rejects" },
        { false, true, false, "out-of-range locked rejects" },

        // Residual 2801 pins (full 2^2 surface).
        { true, false, true, "residual in-range unlocked" },
        { true, true, false, "residual in-range locked" },
        { false, false, false, "residual out-of-range unlocked" },
        { false, true, false, "residual out-of-range locked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowSetItem(c.slotInRange, c.locked);
        const bool inlineF = inlineShouldAllowSetItem(c.slotInRange, c.locked);
        const bool wantPin = c.slotInRange && !c.locked;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldAllowSetItem dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldAllowSetItem == pin formula slotInRange && !locked") && ok;
    }

    // Pin composition: only in-range && !locked admits.
    ok = expect(ShouldAllowSetItem(true, false), "in-range unlocked must accept") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "in-range locked must reject") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "out-of-range unlocked must reject") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "out-of-range locked must reject") && ok;

    // Dense compose: full 2^2 boolean space.
    for (const bool slotInRange : { false, true })
    {
        for (const bool locked : { false, true })
        {
            const bool got  = ShouldAllowSetItem(slotInRange, locked);
            const bool want = slotInRange && !locked;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldAllowSetItem(slotInRange, locked),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CUContainer::SetItem path semantics ---
    // Host injects:
    //   slotInRange = slotID < m_PItem.size()
    //   locked      = m_lock
    // when true  → PlanSetItemCountDelta + assign; return true
    // when false → leave state; return false
    ok = expect(ShouldAllowSetItem(true, false), "in-range unlocked → accept path") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "out-of-range inject → reject path") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "locked inject → reject path") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "out-of-range locked inject → reject path") && ok;

    // Host-style inject poles (mirrors universal_container.cpp probes).
    for (const auto& pole : {
             // slotInRange, locked
             std::tuple<bool, bool>{ false, false }, // out of range unlocked
             std::tuple<bool, bool>{ false, true },  // out of range locked
             std::tuple<bool, bool>{ true, true },   // in range locked
             std::tuple<bool, bool>{ true, false },  // admit
         })
    {
        const bool slotInRange = std::get<0>(pole);
        const bool locked      = std::get<1>(pole);
        const bool inject      = slotInRange && !locked;
        ok                     = expect(ShouldAllowSetItem(slotInRange, locked) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(ShouldAllowSetItem(slotInRange, locked) ==
                        inlineShouldAllowSetItem(slotInRange, locked),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
