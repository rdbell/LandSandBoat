#include "test_casket_drop_threshold_3155.h"

#include "map/casket_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "casket DropThreshold 3155 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua utils.clamp for dual-wire cross-check.
auto inlineClamp(const double v, const double lo, const double hi) -> double
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

// Inline Lua dropChance threshold for dual-wire cross-check (dedicated 3155):
//   utils.clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0, 1)
auto inlineDropThreshold(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower) -> double
{
    return inlineClamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0.0, 1.0);
}

// Compact dual-wire pin matching Go pinDropThreshold3155 / C++ capacity:
//   Clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0, 1)
auto pinDropThreshold(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower) -> double
{
    const double sum = baseRate + kupowersMMBPower + prowessCasketsPower;
    if (sum < 0.0)
    {
        return 0.0;
    }
    if (sum > 1.0)
    {
        return 1.0;
    }
    return sum;
}

auto nearlyEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

} // namespace

// Pure dual-wire expansion for caskethelpers::DropThreshold
// (Lua caskets.lua dropChance threshold inject form;
// OmegaXI internal/casket; slice 3155).
//
// Coverage:
//   - poles 0 / 1 / overflow / underflow / partials
//   - free == inline == pin == clamp(base + kupo + prowess, 0, 1)
//   - residual 0917 / 2878 pins still hold
//   - sibling DropChance residual composes via DropThreshold
auto runCasketDropThreshold3155SelfTests() -> bool
{
    using caskethelpers::DropChance;
    using caskethelpers::DropThreshold;

    bool ok = true;

    // Residual 0917 / 2878 pins still hold under dual-wire.
    ok = expect(nearlyEqual(DropThreshold(0.1, 0.0, 0.0), 0.1), "residual: default base only") && ok;
    ok = expect(nearlyEqual(DropThreshold(0.1, 0.0, 0.20), 0.30), "residual: default + max prowess") && ok;
    ok = expect(nearlyEqual(DropThreshold(0.1, 0.1, 0.20), 0.4), "residual: base + both modifiers") && ok;
    ok = expect(nearlyEqual(DropThreshold(-0.5, 0.0, 0.0), 0.0), "residual: negative base floor") && ok;
    ok = expect(nearlyEqual(DropThreshold(1.5, 0.0, 0.0), 1.0), "residual: base 1.5 ceiling") && ok;
    ok = expect(nearlyEqual(DropThreshold(0.9, 0.2, 0.1), 1.0), "residual: sum overshoot ceiling") && ok;

    // --- Core poles: free == inline == pin == clamp(sum, 0, 1) ---
    const struct
    {
        double      base;
        double      kupo;
        double      prowess;
        double      want;
        const char* label;
    } cases[] = {
        // Pole 0 (floor / all-zero / underflow).
        { 0.0, 0.0, 0.0, 0.0, "all zero → 0" },
        { -0.5, 0.0, 0.0, 0.0, "negative base underflow → 0" },
        { 0.1, -0.5, 0.0, 0.0, "0.1-0.5 underflow → 0" },
        { 0.05, -0.1, 0.0, 0.0, "0.05-0.1 underflow → 0" },
        { -1.0, -1.0, -1.0, 0.0, "triple negative underflow → 0" },
        { 0.0, -0.01, 0.0, 0.0, "tiny negative sum underflow → 0" },

        // Pole 1 (exact ceiling / exact unit).
        { 1.0, 0.0, 0.0, 1.0, "base 1 exact → 1" },
        { 0.5, 0.5, 0.0, 1.0, "0.5+0.5 exact 1" },
        { 0.0, 0.0, 1.0, 1.0, "prowess-only 1" },
        { 0.0, 1.0, 0.0, 1.0, "kupowers-only 1" },

        // Overflow (sum > 1 → clamp 1).
        { 1.5, 0.0, 0.0, 1.0, "base 1.5 overflow → 1" },
        { 0.9, 0.2, 0.1, 1.0, "0.9+0.2+0.1 overflow → 1" },
        { 1.0, 0.1, 0.0, 1.0, "1+0.1 overflow → 1" },
        { 2.0, 2.0, 2.0, 1.0, "large triple overflow → 1" },
        { 0.6, 0.6, 0.6, 1.0, "0.6*3 overflow → 1" },

        // Partials (in-range sum pass-through).
        { 0.1, 0.0, 0.0, 0.1, "default base only partial" },
        { 0.25, 0.0, 0.0, 0.25, "base 0.25 pass-through" },
        { 0.1, 0.0, 0.05, 0.15, "base + prowess 0.05 partial" },
        { 0.1, 0.1, 0.0, 0.2, "base + kupowers 0.1 partial" },
        { 0.1, 0.0, 0.20, 0.30, "default + max prowess partial" },
        { 0.1, 0.1, 0.20, 0.4, "base + both modifiers partial" },
        { 0.1, 0.0, 0.04, 0.14, "default + prowess stack1 partial" },
        { 0.01, 0.02, 0.03, 0.06, "tiny partials sum" },
        { 0.5, 0.0, 0.0, 0.5, "mid base partial" },

        // Residual 2878 / 0917 re-pins.
        { 0.1, 0.0, 0.0, 0.1, "residual 2878 default base" },
        { 0.1, 0.0, 0.20, 0.30, "residual 2878 max prowess" },
        { -0.5, 0.0, 0.0, 0.0, "residual 0917 floor" },
        { 1.5, 0.0, 0.0, 1.0, "residual 0917 ceiling" },
    };

    for (const auto& c : cases)
    {
        const double got     = DropThreshold(c.base, c.kupo, c.prowess);
        const double inlineF = inlineDropThreshold(c.base, c.kupo, c.prowess);
        const double pinGot  = pinDropThreshold(c.base, c.kupo, c.prowess);

        ok = expect(nearlyEqual(got, c.want), c.label) && ok;
        ok = expect(nearlyEqual(got, inlineF), "dual-wire free==inline") && ok;
        ok = expect(nearlyEqual(got, pinGot), "dual-wire free==pin") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(nearlyEqual(DropThreshold(0.1, 0.0, 0.20), pinDropThreshold(0.1, 0.0, 0.20)),
                "free==pin default + max prowess") &&
         ok;
    ok = expect(nearlyEqual(DropThreshold(0.0, 0.0, 0.0), pinDropThreshold(0.0, 0.0, 0.0)),
                "free==pin all zero") &&
         ok;
    ok = expect(nearlyEqual(DropThreshold(2.0, 0.0, 0.0), pinDropThreshold(2.0, 0.0, 0.0)),
                "free==pin overflow") &&
         ok;
    ok = expect(nearlyEqual(DropThreshold(-1.0, 0.0, 0.0), pinDropThreshold(-1.0, 0.0, 0.0)),
                "free==pin underflow") &&
         ok;

    // Dense compose over representative scalar poles — free == inline == pin.
    const double bases[]     = { -1.0, -0.5, 0.0, 0.1, 0.5, 1.0, 1.5, 2.0 };
    const double kupos[]     = { -0.5, 0.0, 0.1, 0.5, 1.0 };
    const double prowesses[] = { -0.1, 0.0, 0.04, 0.05, 0.20, 0.5, 1.0 };
    for (const double base : bases)
    {
        for (const double kupo : kupos)
        {
            for (const double prowess : prowesses)
            {
                const double got     = DropThreshold(base, kupo, prowess);
                const double inlineF = inlineDropThreshold(base, kupo, prowess);
                const double pinGot  = pinDropThreshold(base, kupo, prowess);
                ok                   = expect(nearlyEqual(got, inlineF), "compose free==inline") && ok;
                ok                   = expect(nearlyEqual(got, pinGot), "compose free==pin") && ok;
                ok                   = expect(got >= 0.0 && got <= 1.0, "compose in [0,1]") && ok;
            }
        }
    }

    // Production path semantics: free function is the threshold surface.
    const double thr = DropThreshold(0.1, 0.0, 0.20);
    ok               = expect(nearlyEqual(thr, 0.30), "production sample threshold ~0.30") && ok;
    ok               = expect(nearlyEqual(thr, pinDropThreshold(0.1, 0.0, 0.20)), "production sample free==pin") && ok;

    // Sibling DropChance residual still composes via DropThreshold (not
    // re-expanded under 3155; residual 2878 owns dedicated DropChance suite).
    ok = expect(!DropChance(0.1, 0.0, 0.20, thr), "sibling residual: roll == threshold false") && ok;
    ok = expect(DropChance(0.1, 0.0, 0.20, thr - 1e-9), "sibling residual: roll just under true") && ok;

    return ok;
}
