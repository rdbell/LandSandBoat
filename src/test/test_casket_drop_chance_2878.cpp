#include "test_casket_drop_chance_2878.h"

#include "map/casket_capacity.h"

#include <cmath>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "casket DropChance 2878 self-test failed: " << label << '\n';
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

// Inline Lua dropChance threshold for dual-wire cross-check:
//   utils.clamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0, 1)
auto inlineDropThreshold(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower) -> double
{
    return inlineClamp(baseRate + kupowersMMBPower + prowessCasketsPower, 0.0, 1.0);
}

// Inline Lua dropChance gate: roll < threshold (strict <).
auto inlineDropChance(const double baseRate, const double kupowersMMBPower, const double prowessCasketsPower, const double roll) -> bool
{
    return roll < inlineDropThreshold(baseRate, kupowersMMBPower, prowessCasketsPower);
}

auto nearlyEqual(const double a, const double b) -> bool
{
    return std::fabs(a - b) < 1e-12;
}

} // namespace

// Pure dual-wire expansion for caskethelpers::DropThreshold / DropChance
// (Lua caskets.lua dropChance inject form).
auto runCasketDropChance2878SelfTests() -> bool
{
    using caskethelpers::Clamp;
    using caskethelpers::DropChance;
    using caskethelpers::DropThreshold;

    bool ok = true;

    // Clamp helper pins (0..1 range used by drop product).
    ok = expect(nearlyEqual(Clamp(-0.5, 0.0, 1.0), 0.0), "Clamp floor -0.5 → 0") && ok;
    ok = expect(nearlyEqual(Clamp(1.5, 0.0, 1.0), 1.0), "Clamp ceiling 1.5 → 1") && ok;
    ok = expect(nearlyEqual(Clamp(0.25, 0.0, 1.0), 0.25), "Clamp pass-through 0.25") && ok;
    ok = expect(nearlyEqual(Clamp(0.0, 0.0, 1.0), 0.0), "Clamp exact 0") && ok;
    ok = expect(nearlyEqual(Clamp(1.0, 0.0, 1.0), 1.0), "Clamp exact 1") && ok;
    ok = expect(nearlyEqual(Clamp(0.5, 0.0, 1.0), inlineClamp(0.5, 0.0, 1.0)),
                "Clamp dual-wire == inline") &&
         ok;

    // DropThreshold: base-only, modifiers, clamp 0..1.
    const struct
    {
        double      base;
        double      kupo;
        double      prowess;
        double      want;
        const char* label;
    } thrCases[] = {
        { 0.1, 0.0, 0.0, 0.1, "default base only" },
        { 0.25, 0.0, 0.0, 0.25, "base 0.25 pass-through" },
        { 0.0, 0.0, 0.0, 0.0, "all zero" },
        { 1.0, 0.0, 0.0, 1.0, "base 1" },
        { 0.1, 0.0, 0.05, 0.15, "base + prowess 0.05" },
        { 0.1, 0.1, 0.0, 0.2, "base + kupowers 0.1" },
        { 0.1, 0.0, 0.20, 0.30, "default + max prowess stack" },
        { 0.1, 0.1, 0.20, 0.4, "base + both modifiers" },
        { 0.1, 0.0, 0.04, 0.14, "default + prowess stack1" },
        { -0.5, 0.0, 0.0, 0.0, "negative base floor" },
        { 0.1, -0.5, 0.0, 0.0, "0.1-0.5 floor" },
        { 0.05, -0.1, 0.0, 0.0, "0.05-0.1 floor" },
        { 1.5, 0.0, 0.0, 1.0, "base 1.5 ceiling" },
        { 0.9, 0.2, 0.1, 1.0, "0.9+0.2+0.1 ceiling" },
        { 1.0, 0.1, 0.0, 1.0, "1+0.1 ceiling" },
        { 0.5, 0.5, 0.0, 1.0, "0.5+0.5 exact 1" },
    };

    for (const auto& c : thrCases)
    {
        const double got     = DropThreshold(c.base, c.kupo, c.prowess);
        const double inlineF = inlineDropThreshold(c.base, c.kupo, c.prowess);

        ok = expect(nearlyEqual(got, c.want), c.label) && ok;
        ok = expect(nearlyEqual(got, inlineF), "DropThreshold dual-wire == inline") && ok;
    }

    // DropChance: strict < (roll == threshold → false).
    ok = expect(!DropChance(0.1, 0.0, 0.0, 0.1), "roll == threshold false") && ok;
    ok = expect(DropChance(0.1, 0.0, 0.0, 0.099999), "roll just under threshold true") && ok;
    ok = expect(DropChance(0.1, 0.0, 0.0, 0.0), "roll 0 with rate > 0 true") && ok;
    ok = expect(!DropChance(0.0, 0.0, 0.0, 0.0), "threshold 0 roll 0 false (0 < 0)") && ok;
    ok = expect(!DropChance(0.0, 0.0, 0.0, 0.5), "threshold 0 roll 0.5 false") && ok;
    ok = expect(DropChance(1.0, 0.0, 0.0, 0.999), "threshold 1 roll 0.999 true") && ok;
    ok = expect(!DropChance(1.0, 0.0, 0.0, 1.0), "threshold 1 roll 1 false (strict <)") && ok;
    ok = expect(DropChance(2.0, 0.0, 0.0, 0.5), "clamped threshold 1 roll 0.5 true") && ok;

    // Edge: roll equal to combined threshold is false.
    const double thr = DropThreshold(0.1, 0.0, 0.20);
    ok = expect(nearlyEqual(thr, 0.30), "combined threshold ~0.30") && ok;
    ok = expect(DropChance(0.1, 0.0, 0.20, thr - 1e-9), "roll just under combined true") && ok;
    ok = expect(!DropChance(0.1, 0.0, 0.20, thr), "roll == combined threshold false") && ok;

    // DropChance dual-wire matches inline formula for sample injects.
    const struct
    {
        double base;
        double kupo;
        double prowess;
        double roll;
    } chanceCases[] = {
        { 0.1, 0.0, 0.0, 0.05 },
        { 0.1, 0.0, 0.0, 0.1 },
        { 0.1, 0.1, 0.2, 0.39 },
        { 0.0, 0.0, 0.0, 0.0 },
        { -1.0, 0.0, 0.0, 0.0 },
        { 2.0, 0.0, 0.0, 0.999 },
        { 0.1, 0.0, 0.20, 0.30 },
        { 0.1, 0.0, 0.20, 0.299999 },
    };

    for (const auto& c : chanceCases)
    {
        const bool got     = DropChance(c.base, c.kupo, c.prowess, c.roll);
        const bool inlineF = inlineDropChance(c.base, c.kupo, c.prowess, c.roll);
        const bool want    = c.roll < DropThreshold(c.base, c.kupo, c.prowess);

        ok = expect(got == want, "DropChance == roll < DropThreshold") && ok;
        ok = expect(got == inlineF, "DropChance dual-wire == inline") && ok;
    }

    return ok;
}
