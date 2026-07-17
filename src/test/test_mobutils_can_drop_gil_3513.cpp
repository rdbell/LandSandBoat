#include "test_mobutils_can_drop_gil_3513.h"

#include "map/mobutils_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils CanDropGil 3513 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanDropGil formula for dual-wire cross-check (slice 3513):
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
// Direct return form matching production free function / capacity.
auto inlineCanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

// Compact dual-wire pin matching free function / capacity body (slice 3513).
// Direct return only — same formula as production CanDropGil.
auto pinCanDropGil3513(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

} // namespace

// Pure dual-wire expansion for mobutilshelpers::CanDropGil
// (gilMin/gilMax/gilBonus eligibility gate; slice 3513 dedicated expand
// residual 2960). CanStealGil dual-wires through the same free function.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 2960 / 2653 / prior dedicated 3158 / prior expand 3231 / 3279 /
//     3309 poles
//   - residual poles: negative max suppress, zero all, min/max/bonus only
//   - dense min/max/bonus edges: negatives, zero, max int16
auto runMobutilsCanDropGil3513SelfTests() -> bool
{
    using mobutilshelpers::CanDropGil;
    using mobutilshelpers::CanStealGil;

    bool ok = true;

    // Residual 2653 / residual dual-wire 2960 / prior dedicated 3158 /
    // prior dedicated expand residual 2960 3231 / 3279 / 3309 pins.
    ok = expect(!CanDropGil(0, 0, 0), "residual all zero rejects") && ok;
    ok = expect(CanDropGil(1, 0, 0), "residual positive min permits") && ok;
    ok = expect(CanDropGil(0, 1, 0), "residual positive max permits") && ok;
    ok = expect(CanDropGil(0, 0, 1), "residual positive bonus permits") && ok;
    ok = expect(!CanDropGil(1, -1, 1), "residual negative max suppresses") && ok;
    ok = expect(!CanDropGil(0, 0, -1), "residual negative bonus rejects") && ok;

    // Explicit required poles (direct return pin form).
    ok = expect(!CanDropGil(1, -1, 1), "negative max suppress min+bonus") && ok;
    ok = expect(!CanDropGil(0, -1, 0), "negative max alone suppress") && ok;
    ok = expect(!CanDropGil(100, -5, 100), "negative max suppress large") && ok;
    ok = expect(!CanDropGil(0, -32768, 0), "negative max int16 min suppress") && ok;
    ok = expect(!CanDropGil(0, 0, 0), "zero all reject") && ok;
    ok = expect(CanDropGil(1, 0, 0), "min only permit") && ok;
    ok = expect(CanDropGil(0, 1, 0), "max only permit") && ok;
    ok = expect(CanDropGil(0, 0, 1), "bonus only permit") && ok;

    const struct
    {
        int16       gilMin;
        int16       gilMax;
        int16       gilBonus;
        bool        want;
        const char* label;
    } cases[] = {
        // Required residual poles.
        { 1, -1, 1, false, "negative max suppress min+bonus" },
        { 0, -1, 0, false, "negative max alone suppress" },
        { 100, -5, 100, false, "negative max suppress large" },
        { 0, -32768, 0, false, "negative max int16 min suppress" },
        { 0, 0, 0, false, "zero all reject" },
        { 1, 0, 0, true, "min only permit" },
        { 0, 1, 0, true, "max only permit" },
        { 0, 0, 1, true, "bonus only permit" },

        // Boundaries and combined paths.
        { 0, 0, -1, false, "negative bonus does not permit" },
        { -1, 0, 0, false, "negative min with zero max rejects" },
        { 0, 5, 0, true, "nonzero max alone permits" },
        { -1, 5, -1, true, "positive max permits despite neg min/bonus" },
        { 0, 32767, 0, true, "int16 max max permits" },
        { 2, 0, 0, true, "min > 1 permits" },
        { 32767, 0, 0, true, "int16 max min permits" },
        { 0, 0, 100, true, "large bonus permits" },
        { 0, 0, 32767, true, "int16 max bonus permits" },
        { 1, 1, 1, true, "all positive permits" },
        { 10, 20, 5, true, "typical gil range permits" },
        { 1, 0, -1, true, "positive min permits despite neg bonus" },
        { 0, 1, -1, true, "positive max permits despite neg bonus" },

        // Dense min/max/bonus edges: negatives, zero, max int16.
        { -32768, -32768, -32768, false, "dense all int16 min" },
        { -32768, 0, 0, false, "dense min int16 min zero max/bonus" },
        { 0, -32768, 0, false, "dense max int16 min suppress" },
        { 0, 0, -32768, false, "dense bonus int16 min reject" },
        { 32767, 32767, 32767, true, "dense all int16 max" },
        { 32767, 0, 0, true, "dense min int16 max only" },
        { 0, 32767, 0, true, "dense max int16 max only" },
        { 0, 0, 32767, true, "dense bonus int16 max only" },
        { -1, -1, -1, false, "dense all -1 (neg max suppress)" },
        { -1, 0, -1, false, "dense neg min/bonus zero max" },
        { 0, -1, 0, false, "dense max -1 suppress" },
        { 1, 0, 0, true, "dense min 1 only" },
        { 0, 1, 0, true, "dense max 1 only" },
        { 0, 0, 1, true, "dense bonus 1 only" },
        { -32768, 32767, -32768, true, "dense max int16 max permits despite edges" },
        { 32767, -1, 32767, false, "dense neg max suppress int16 max min+bonus" },

        // Residual 2960 / 2653 / prior 3158 / prior expand 3231 / 3279 / 3309 re-pins.
        { 0, 0, 0, false, "residual 2960/2653/3158/3231/3279/3309 all zero" },
        { 1, 0, 0, true, "residual 2960/2653/3158/3231/3279/3309 positive min" },
        { 0, 1, 0, true, "residual 2960/2653/3158/3231/3279/3309 positive max" },
        { 0, 0, 1, true, "residual 2960/2653/3158/3231/3279/3309 positive bonus" },
        { 1, -1, 1, false, "residual 2960/2653/3158/3231/3279/3309 neg max suppress" },
        { 0, 0, -1, false, "residual 2960/2653/3158/3231/3279/3309 neg bonus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDropGil(c.gilMin, c.gilMax, c.gilBonus);
        const bool inlineF = inlineCanDropGil(c.gilMin, c.gilMax, c.gilBonus);
        const bool pin     = pinCanDropGil3513(c.gilMin, c.gilMax, c.gilBonus);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanDropGil dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "CanDropGil dual-wire == direct pin") && ok;

        // CanStealGil dual-wires through the same free function.
        const bool steal = CanStealGil(c.gilMin, c.gilMax, c.gilBonus);
        ok               = expect(steal == got, "CanStealGil == CanDropGil") && ok;
        ok               = expect(steal == pin, "CanStealGil dual-wire == direct pin") && ok;
    }

    // Free == pin across residual poles (direct return form).
    ok = expect(CanDropGil(1, 0, 0) == pinCanDropGil3513(1, 0, 0), "free == pin residual min only") && ok;
    ok = expect(CanDropGil(0, 1, 0) == pinCanDropGil3513(0, 1, 0), "free == pin residual max only") && ok;
    ok = expect(CanDropGil(0, 0, 1) == pinCanDropGil3513(0, 0, 1), "free == pin residual bonus only") && ok;
    ok = expect(CanDropGil(0, 0, 0) == pinCanDropGil3513(0, 0, 0), "free == pin residual zero all") && ok;
    ok = expect(CanDropGil(1, -1, 1) == pinCanDropGil3513(1, -1, 1),
                "free == pin residual negative max suppress") &&
         ok;

    // Dense compose over min/max/bonus edges including negatives, zero,
    // max int16: free == pin == inline (direct return).
    const int16 poles[] = { -32768, -1, 0, 1, 2, 100, 32767 };
    for (const int16 gilMin : poles)
    {
        for (const int16 gilMax : poles)
        {
            for (const int16 gilBonus : poles)
            {
                const bool got = CanDropGil(gilMin, gilMax, gilBonus);
                const bool pin = pinCanDropGil3513(gilMin, gilMax, gilBonus);
                const bool inl = inlineCanDropGil(gilMin, gilMax, gilBonus);
                ok             = expect(got == pin, "compose free == direct pin") && ok;
                ok             = expect(got == inl, "compose free == inline") && ok;
                ok = expect(CanStealGil(gilMin, gilMax, gilBonus) == got, "compose steal == drop") && ok;
            }
        }
    }

    // Host-style inject poles: getMobMod(MOBMOD_GIL_MIN/MAX/BONUS) as int16.
    // (Live CMobEntity::CanDropGil / death gil path is residual 2653 / entity.)
    const struct
    {
        int16 gilMin;
        int16 gilMax;
        int16 gilBonus;
        bool  want;
    } hostPoles[] = {
        { 0, 0, 0, false },
        { 1, 0, 0, true },
        { 0, 1, 0, true },
        { 0, 0, 1, true },
        { 1, -1, 1, false },
        { 10, 20, 0, true },
        { 0, -1, 0, false },
        { -32768, 0, 0, false },
        { 0, 32767, 0, true },
        { 0, 0, 32767, true },
        { 32767, -32768, 32767, false },
    };
    for (const auto& h : hostPoles)
    {
        const bool got = CanDropGil(h.gilMin, h.gilMax, h.gilBonus);
        const bool pin = pinCanDropGil3513(h.gilMin, h.gilMax, h.gilBonus);
        const bool inl = inlineCanDropGil(h.gilMin, h.gilMax, h.gilBonus);
        ok             = expect(got == pin, "host inject free == direct pin") && ok;
        ok             = expect(got == inl, "host inject free == inline") && ok;
        ok             = expect(got == h.want, "host inject want pin") && ok;
        ok = expect(CanStealGil(h.gilMin, h.gilMax, h.gilBonus) == h.want, "host steal inject want") && ok;
    }

    // Production path semantics:
    //   CanDropGil(getMobMod MIN, getMobMod MAX, getMobMod BONUS)
    // when true  → death / steal may award gil (subject to multiplier hosts)
    // when false → no gil drop / no steal gil
    ok = expect(!CanDropGil(0, 0, 0), "no mods → no drop path") && ok;
    ok = expect(CanDropGil(1, 100, 0), "range mods → drop path") && ok;
    ok = expect(!CanDropGil(50, -1, 10), "suppressed max → no drop path") && ok;
    ok = expect(CanStealGil(1, 100, 0) == CanDropGil(1, 100, 0), "steal delegates drop path") && ok;

    return ok;
}
