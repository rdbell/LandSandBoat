#include "test_universalcontainer_set_item_3319.h"

#include "map/universal_container_capacity.h"

#include <iostream>
#include <tuple>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "universalcontainer ShouldAllowSetItem 3319 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SetItem outer gate for dual-wire cross-check (dedicated 3319):
//   slotInRange && !locked
// Positive form only (avoid De Morgan compound-not rewrites).
auto inlineShouldAllowSetItem(const bool slotInRange, const bool locked) -> bool
{
    return slotInRange && !locked;
}

// Compact dual-wire pin matching Go pinShouldAllowSetItem3319 / C++ capacity:
//   slotInRange && !locked
// Positive form only (avoid De Morgan compound-not rewrites).
auto pinShouldAllowSetItem(const bool slotInRange, const bool locked) -> bool
{
    return slotInRange && !locked;
}

} // namespace

// Pure dual-wire expansion for ucontainerhelpers::ShouldAllowSetItem
// (SetItem outer gate; OmegaXI internal/universalcontainer;
// dedicated slice 3319 expand residual 2965 / prior 3288).
//
// Coverage:
//   - free == inline == pin == (slotInRange && !locked)
//   - residual poles (2801 / 2965 / prior 3176 / prior 3248 / prior 3288) still hold
//   - dense 2^2 free == inline == pin
auto runUniversalcontainerSetItem3319SelfTests() -> bool
{
    using ucontainerhelpers::ShouldAllowSetItem;

    bool ok = true;

    // Residual 2801 / 2965 / prior 3176 / prior 3248 / prior 3288 pins still hold under dual-wire.
    ok = expect(ShouldAllowSetItem(true, false), "residual: in-range unlocked accepts") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "residual: in-range locked rejects") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "residual: out-of-range unlocked rejects") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "residual: out-of-range locked rejects") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool        slotInRange;
        bool        locked;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles / residual 2801+2965+prior 3176+prior 3248+prior 3288 surface.
        { true, false, true, "in-range unlocked accepts" },
        { true, true, false, "in-range locked rejects" },
        { false, false, false, "out-of-range unlocked rejects" },
        { false, true, false, "out-of-range locked rejects" },

        // Residual 2801 / 2965 / prior 3176 / prior 3248 / prior 3288 pins (full 2^2 surface, labeled).
        { true, false, true, "residual in-range unlocked" },
        { true, true, false, "residual in-range locked" },
        { false, false, false, "residual out-of-range unlocked" },
        { false, true, false, "residual out-of-range locked" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldAllowSetItem(c.slotInRange, c.locked);
        const bool inlineF = inlineShouldAllowSetItem(c.slotInRange, c.locked);
        const bool pin     = pinShouldAllowSetItem(c.slotInRange, c.locked);
        // Positive pin form: slotInRange && !locked
        const bool wantPin = c.slotInRange && !c.locked;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula slotInRange && !locked") && ok;
    }

    // Direct return pin: only in-range && !locked admits.
    ok = expect(ShouldAllowSetItem(true, false), "in-range unlocked must accept") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "in-range locked must reject") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "out-of-range unlocked must reject") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "out-of-range locked must reject") && ok;
    // inline+pin direct return identity on admit/reject poles.
    ok = expect(inlineShouldAllowSetItem(true, false) && pinShouldAllowSetItem(true, false),
                "inline/pin direct return must accept in-range unlocked") &&
         ok;
    ok = expect(!inlineShouldAllowSetItem(true, true) && !pinShouldAllowSetItem(true, true),
                "inline/pin direct return must reject in-range locked") &&
         ok;

    // Dense compose: full 2^2 boolean space free == inline == pin.
    for (const bool slotInRange : { false, true })
    {
        for (const bool locked : { false, true })
        {
            const bool got     = ShouldAllowSetItem(slotInRange, locked);
            const bool inlineF = inlineShouldAllowSetItem(slotInRange, locked);
            const bool pin     = pinShouldAllowSetItem(slotInRange, locked);
            const bool want    = slotInRange && !locked;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Host-style inject poles: free == inline == pin for SetItem.
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
        const bool got         = ShouldAllowSetItem(slotInRange, locked);
        const bool inlineF     = inlineShouldAllowSetItem(slotInRange, locked);
        const bool pin         = pinShouldAllowSetItem(slotInRange, locked);
        const bool inject      = slotInRange && !locked;
        ok                     = expect(got == inject, "host inject dual-wire identity") && ok;
        ok                     = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // Production SetItem path semantics:
    //   slotInRange = slotID < m_PItem.size()
    //   locked      = m_lock
    // when true  → PlanSetItemCountDelta + assign; return true
    // when false → leave state; return false
    ok = expect(ShouldAllowSetItem(true, false), "in-range unlocked → accept path") && ok;
    ok = expect(!ShouldAllowSetItem(false, false), "out-of-range inject → reject path") && ok;
    ok = expect(!ShouldAllowSetItem(true, true), "locked inject → reject path") && ok;
    ok = expect(!ShouldAllowSetItem(false, true), "out-of-range locked inject → reject path") && ok;

    return ok;
}
