#include "test_mobutils_can_drop_gil_2960.h"

#include "map/mobutils_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mobutils CanDropGil 2960 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CMobEntity::CanDropGil formula for dual-wire cross-check (slice 2960):
//   gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0)
auto inlineCanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

} // namespace

// Pure dual-wire expansion for mobutilshelpers::CanDropGil
// (gilMin/gilMax/gilBonus eligibility gate; slice 2960).
// CanStealGil dual-wires through the same free function.
auto runMobutilsCanDropGil2960SelfTests() -> bool
{
    using mobutilshelpers::CanDropGil;
    using mobutilshelpers::CanStealGil;

    bool ok = true;

    // Residual 2653 pins still hold under dual-wire.
    ok = expect(!CanDropGil(0, 0, 0), "residual all zero rejects") && ok;
    ok = expect(CanDropGil(1, 0, 0), "residual positive min permits") && ok;
    ok = expect(CanDropGil(0, 1, 0), "residual positive max permits") && ok;
    ok = expect(CanDropGil(0, 0, 1), "residual positive bonus permits") && ok;
    ok = expect(!CanDropGil(1, -1, 1), "residual negative max suppresses") && ok;
    ok = expect(!CanDropGil(0, 0, -1), "residual negative bonus rejects") && ok;

    const struct
    {
        int16       gilMin;
        int16       gilMax;
        int16       gilBonus;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { 0, 0, 0, false, "all zero modifiers reject" },
        { 1, 0, 0, true, "positive minimum permits drop" },
        { 0, 1, 0, true, "positive maximum permits drop" },
        { 0, 0, 1, true, "positive bonus permits drop" },

        // Negative max suppression sentinel (takes precedence).
        { 1, -1, 1, false, "negative max suppresses min+bonus" },
        { 0, -1, 0, false, "negative max alone suppresses" },
        { 100, -5, 100, false, "negative max suppresses large min+bonus" },
        { 0, -32768, 0, false, "int16 min max suppresses" },

        // Max zero with other gates.
        { 0, 0, -1, false, "negative bonus does not permit" },
        { -1, 0, 0, false, "negative min with zero max rejects" },
        { 0, 0, 0, false, "zero/zero/zero rejects" },

        // Positive max with zero/negative min/bonus.
        { 0, 5, 0, true, "nonzero max alone permits" },
        { -1, 5, -1, true, "positive max permits despite neg min/bonus" },
        { 0, 32767, 0, true, "int16 max max permits" },

        // Positive min boundaries.
        { 1, 0, 0, true, "min == 1 permits" },
        { 2, 0, 0, true, "min > 1 permits" },
        { 32767, 0, 0, true, "int16 max min permits" },

        // Bonus boundaries.
        { 0, 0, 1, true, "bonus == 1 permits" },
        { 0, 0, 100, true, "large bonus permits" },
        { 0, 0, 32767, true, "int16 max bonus permits" },

        // Combined positive paths.
        { 1, 1, 1, true, "all positive permits" },
        { 10, 20, 5, true, "typical gil range permits" },
        { 1, 0, -1, true, "positive min permits despite neg bonus" },
        { 0, 1, -1, true, "positive max permits despite neg bonus" },

        // Residual 2653 re-pins.
        { 0, 0, 0, false, "residual all zero" },
        { 1, 0, 0, true, "residual positive min" },
        { 0, 1, 0, true, "residual positive max" },
        { 0, 0, 1, true, "residual positive bonus" },
        { 1, -1, 1, false, "residual neg max suppress" },
        { 0, 0, -1, false, "residual neg bonus" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanDropGil(c.gilMin, c.gilMax, c.gilBonus);
        const bool inlineF = inlineCanDropGil(c.gilMin, c.gilMax, c.gilBonus);
        const bool wantPin = c.gilMax >= 0 && (c.gilMin > 0 || c.gilMax != 0 || c.gilBonus > 0);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanDropGil dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanDropGil == pin formula") && ok;

        // CanStealGil dual-wires through the same free function.
        const bool steal = CanStealGil(c.gilMin, c.gilMax, c.gilBonus);
        ok               = expect(steal == got, "CanStealGil == CanDropGil") && ok;
        ok               = expect(steal == inlineF, "CanStealGil dual-wire == inline") && ok;
    }

    // Pin composition: free function is the three-mod eligibility gate only.
    ok = expect(!CanDropGil(0, 0, 0), "0,0,0 must reject") && ok;
    ok = expect(CanDropGil(1, 0, 0), "1,0,0 must permit") && ok;
    ok = expect(CanDropGil(0, 1, 0), "0,1,0 must permit") && ok;
    ok = expect(CanDropGil(0, 0, 1), "0,0,1 must permit") && ok;
    ok = expect(!CanDropGil(1, -1, 1), "neg max must suppress") && ok;

    // Dense compose over representative poles: free == pin == inline.
    const int16 poles[] = { -32768, -1, 0, 1, 2, 100, 32767 };
    for (const int16 gilMin : poles)
    {
        for (const int16 gilMax : poles)
        {
            for (const int16 gilBonus : poles)
            {
                const bool got  = CanDropGil(gilMin, gilMax, gilBonus);
                const bool want = gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineCanDropGil(gilMin, gilMax, gilBonus),
                           "compose free == inline") &&
                     ok;
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
    };
    for (const auto& h : hostPoles)
    {
        const bool inject = h.gilMax >= 0 && (h.gilMin > 0 || h.gilMax != 0 || h.gilBonus > 0);
        ok                = expect(CanDropGil(h.gilMin, h.gilMax, h.gilBonus) == inject,
                    "host inject dual-wire identity") &&
             ok;
        ok = expect(CanDropGil(h.gilMin, h.gilMax, h.gilBonus) ==
                        inlineCanDropGil(h.gilMin, h.gilMax, h.gilBonus),
                    "host inject free == inline") &&
             ok;
        ok = expect(CanDropGil(h.gilMin, h.gilMax, h.gilBonus) == h.want, "host inject want pin") && ok;
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
