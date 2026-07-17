#include "test_abyssea_buff_power_3148.h"

#include "map/abyssea_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "abyssea BuffPower 3148 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua visionsCruorProspectorOnEventFinish power formula for dual-wire
// cross-check (dedicated slice 3148):
//   v[3] + getAbyssiteTotal(player, v[4]) * v[5]
auto inlineBuffPower(const int32 base, const int32 abyssiteTotal, const int32 mult) -> int32
{
    return base + abyssiteTotal * mult;
}

// Compact dual-wire pin matching C++ capacity / Go pinBuffPower3148:
//   base + abyssiteTotal * mult
auto pinBuffPower(const int32 base, const int32 abyssiteTotal, const int32 mult) -> int32
{
    return base + abyssiteTotal * mult;
}

} // namespace

// Pure dual-wire expansion for abysseahelpers::BuffPower
// (Lua visionsCruorProspectorOnEventFinish ENHANCEMENT power formula;
// OmegaXI internal/abyssea; slice 3148).
//
// Coverage:
//   - zeros / positives / zero mult / negatives host can pass
//   - free == inline == pin == base + abyssiteTotal * mult
//   - residual 1046 / 2866 pins still hold
//   - sample catalog composition (HP base 20 + merit*10; sel-11)
auto runAbysseaBuffPower3148SelfTests() -> bool
{
    using abysseahelpers::BuffPower;

    bool ok = true;

    // Residual 1046 / 2866 BuffPower pins still hold under dual-wire.
    ok = expect(BuffPower(20, 0, 10) == 20, "residual: HP no merit") && ok;
    ok = expect(BuffPower(20, 1, 10) == 30, "residual: HP 1 merit") && ok;
    ok = expect(BuffPower(20, 6, 10) == 80, "residual: HP full merit span (6)") && ok;
    ok = expect(BuffPower(10, 3, 5) == 25, "residual: MP 3 merit") && ok;
    ok = expect(BuffPower(10, 3, 10) == 40, "residual: stat full furtherance (3)") && ok;
    ok = expect(BuffPower(0, 0, 0) == 0, "residual: all zero") && ok;

    // --- Core poles: free == inline == pin == compose ---
    const struct
    {
        int32       base;
        int32       total;
        int32       mult;
        int32       want;
        const char* label;
    } cases[] = {
        // zeros
        { 0, 0, 0, 0, "all zero" },
        { 0, 5, 10, 50, "zero base" },
        { 10, 0, 10, 10, "zero total" },
        { 10, 5, 0, 10, "zero mult" },
        { 0, 0, 10, 0, "zero base+total" },
        // positives — HP Merit×10
        { 20, 0, 10, 20, "HP no merit" },
        { 20, 1, 10, 30, "HP 1 merit" },
        { 20, 6, 10, 80, "HP full merit span (6)" },
        // positives — MP Merit×5
        { 10, 0, 5, 10, "MP no merit" },
        { 10, 3, 5, 25, "MP 3 merit" },
        { 10, 6, 5, 40, "MP full merit" },
        // positives — Stat Furtherance×10
        { 10, 0, 10, 10, "stat no furtherance" },
        { 10, 1, 10, 20, "stat 1 furtherance" },
        { 10, 3, 10, 40, "stat full furtherance (3)" },
        // unit / large
        { 1, 1, 1, 2, "unit identity" },
        { 100, 10, 10, 200, "large base+total" },
        // negatives host can pass (scalar inject has no sign guard)
        { -5, 2, 10, 15, "negative base" },
        { 20, -1, 10, 10, "negative total" },
        { 20, 2, -5, 10, "negative mult" },
        { -10, -2, 5, -20, "negative base+total" },
        { 10, -3, -2, 16, "negative total+mult" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = BuffPower(c.base, c.total, c.mult);
        const int32 inlineF = inlineBuffPower(c.base, c.total, c.mult);
        const int32 pinGot  = pinBuffPower(c.base, c.total, c.mult);
        const int32 compose = c.base + c.total * c.mult;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==base+abyssiteTotal*mult") && ok;
    }

    // Residual 2866 catalog compose pins (sel-11 sample Merit=2 / Furtherance=1).
    ok = expect(BuffPower(20, 2, 10) == 40, "compose residual: HP merit=2") && ok;
    ok = expect(BuffPower(10, 2, 5) == 20, "compose residual: MP merit=2") && ok;
    ok = expect(BuffPower(10, 1, 10) == 20, "compose residual: stat furtherance=1") && ok;

    // Sample catalog composition: HP base 20 + merit*10 over merit 0..6.
    for (int32 merit = 0; merit <= 6; ++merit)
    {
        const int32 want = 20 + merit * 10;
        const int32 got  = BuffPower(20, merit, 10);
        ok               = expect(got == want, "HP base20 + merit*10") && ok;
        ok               = expect(got == inlineBuffPower(20, merit, 10), "HP free==inline") && ok;
        ok               = expect(got == pinBuffPower(20, merit, 10), "HP free==pin") && ok;
    }

    // MP base 10 + merit*5 over same span.
    for (int32 merit = 0; merit <= 6; ++merit)
    {
        const int32 want = 10 + merit * 5;
        ok               = expect(BuffPower(10, merit, 5) == want, "MP base10 + merit*5") && ok;
    }

    // Stat base 10 + furtherance*10 over furtherance 0..3.
    for (int32 furtherance = 0; furtherance <= 3; ++furtherance)
    {
        const int32 want = 10 + furtherance * 10;
        ok               = expect(BuffPower(10, furtherance, 10) == want, "stat base10 + furtherance*10") && ok;
    }

    // Dense compose identity over representative scalar poles.
    const int32 bases[]  = { -10, -1, 0, 1, 10, 20, 100 };
    const int32 totals[] = { -3, -1, 0, 1, 2, 3, 6, 10 };
    const int32 mults[]  = { -5, -1, 0, 1, 5, 10 };
    for (const int32 base : bases)
    {
        for (const int32 total : totals)
        {
            for (const int32 mult : mults)
            {
                const int32 got  = BuffPower(base, total, mult);
                const int32 want = base + total * mult;
                ok               = expect(got == want, "compose base+abyssiteTotal*mult") && ok;
                ok               = expect(got == inlineBuffPower(base, total, mult), "compose inline") && ok;
                ok               = expect(got == pinBuffPower(base, total, mult), "compose pin") && ok;
            }
        }
    }

    // Production path semantics: free function is the power surface.
    ok = expect(BuffPower(20, 2, 10) == pinBuffPower(20, 2, 10), "formula free==pin") && ok;
    ok = expect(BuffPower(20, 2, 10) == 40, "formula sample HP merit=2 is 40") && ok;

    return ok;
}
