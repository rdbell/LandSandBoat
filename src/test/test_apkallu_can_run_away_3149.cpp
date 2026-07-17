#include "test_apkallu_can_run_away_3149.h"

#include "map/apkallu_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "apkallu can run away 3149 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline xi.apkallu.getHateTier for dual-wire cross-check:
//   hate >= 45 → 3, >= 25 → 2, >= 5 → 1, else 0
auto inlineGetHateTier(const int32 hate) -> int32
{
    if (hate >= 45)
    {
        return 3;
    }
    if (hate >= 25)
    {
        return 2;
    }
    if (hate >= 5)
    {
        return 1;
    }
    return 0;
}

// Inline xi.apkallu.canRunAway pure half once hate is injected:
//   getHateTier(hate) >= 3
auto inlineCanRunAway(const int32 hate) -> bool
{
    return inlineGetHateTier(hate) >= 3;
}

// Compact dual-wire pin matching Go pinCanRunAway3149:
//   GetHateTier(hate) >= 3  ≡  hate >= kHateTier3Min
auto pinCanRunAway(const int32 hate) -> bool
{
    return apkalluhelpers::GetHateTier(hate) >= 3;
}

} // namespace

// Pure dual-wire expansion for apkalluhelpers::CanRunAway / GetHateTier
// (xi.apkallu.canRunAway after zone hate inject; OmegaXI internal/apkallu;
// dedicated slice 3149).
//
// Coverage:
//   - poles around 0 / 4 / 5 / 24 / 25 / 44 / 45 / 100
//   - free == inline == pin == GetHateTier>=3 == hate>=45
//   - residual 0925 / 2880 pins still hold (positive pin form)
auto runApkalluCanRunAway3149SelfTests() -> bool
{
    using apkalluhelpers::CanRunAway;
    using apkalluhelpers::GetHateTier;
    using apkalluhelpers::kHateTier1Min;
    using apkalluhelpers::kHateTier2Min;
    using apkalluhelpers::kHateTier3Min;

    bool ok = true;

    // Threshold constant pins (match Go HateTier{1,2,3}Min).
    ok = expect(kHateTier1Min == 5, "tier1 min pin") && ok;
    ok = expect(kHateTier2Min == 25, "tier2 min pin") && ok;
    ok = expect(kHateTier3Min == 45, "tier3 min pin") && ok;

    // Residual 0925 / 2880 CanRunAway pins still hold under dual-wire
    // (positive pin form for eligible poles).
    ok = expect(!CanRunAway(44), "residual CanRunAway(44)") && ok;
    ok = expect(CanRunAway(45), "residual CanRunAway(45)") && ok;
    ok = expect(CanRunAway(100), "residual CanRunAway(100)") && ok;
    ok = expect(!CanRunAway(0), "residual CanRunAway(0)") && ok;

    // --- Eligible open path (tier3) ---
    ok = expect(CanRunAway(45), "eligible hate 45 (tier3 min)") && ok;
    ok = expect(CanRunAway(46), "eligible hate 46 (tier3 interior)") && ok;
    ok = expect(CanRunAway(100), "eligible hate 100 (hate max)") && ok;
    ok = expect(CanRunAway(999), "eligible hate 999 (over-max)") && ok;
    ok = expect(CanRunAway(kHateTier3Min), "eligible HateTier3Min pin") && ok;

    // --- Blocked paths (tier < 3) ---
    ok = expect(!CanRunAway(-10), "blocked negative") && ok;
    ok = expect(!CanRunAway(0), "blocked hate 0") && ok;
    ok = expect(!CanRunAway(4), "blocked hate 4 (tier0 edge)") && ok;
    ok = expect(!CanRunAway(5), "blocked hate 5 (tier1 min)") && ok;
    ok = expect(!CanRunAway(24), "blocked hate 24 (tier1 max)") && ok;
    ok = expect(!CanRunAway(25), "blocked hate 25 (tier2 min)") && ok;
    ok = expect(!CanRunAway(44), "blocked hate 44 (tier2 max)") && ok;

    // --- Composition table: free == inline == pin == compose ---
    // Poles around 0 / 4 / 5 / 24 / 25 / 44 / 45 / 100.
    const struct
    {
        int32       hate;
        bool        want;
        const char* label;
    } runCases[] = {
        { -10, false, "negative cannot run" },
        { 0, false, "hate 0 cannot run" },
        { 4, false, "tier0 edge (4) cannot run" },
        { 5, false, "tier1 min (5) cannot run" },
        { 24, false, "tier1 max (24) cannot run" },
        { 25, false, "tier2 min (25) cannot run" },
        { 44, false, "tier2 max (44) cannot run" },
        { 45, true, "tier3 min (45) can run" },
        { 46, true, "tier3 interior (46) can run" },
        { 100, true, "hate max (100) can run" },
        { 999, true, "over-max (999) can run" },
    };

    for (const auto& c : runCases)
    {
        const bool got         = CanRunAway(c.hate);
        const bool inlineF     = inlineCanRunAway(c.hate);
        const bool pinGot      = pinCanRunAway(c.hate);
        const bool composeTier = GetHateTier(c.hate) >= 3;
        const bool composeBound = c.hate >= kHateTier3Min;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRunAway dual-wire free == inline") && ok;
        ok = expect(got == pinGot, "CanRunAway dual-wire free == pin") && ok;
        ok = expect(got == composeTier, "CanRunAway == GetHateTier>=3") && ok;
        ok = expect(got == composeBound, "CanRunAway == hate>=45 (tier3 bound)") && ok;
    }

    // Free == pin across residual poles (positive pin form).
    ok = expect(CanRunAway(45) == pinCanRunAway(45), "free == pin residual 45") && ok;
    ok = expect(CanRunAway(100) == pinCanRunAway(100), "free == pin residual 100") && ok;
    ok = expect(CanRunAway(44) == pinCanRunAway(44), "free == pin residual 44") && ok;
    ok = expect(CanRunAway(0) == pinCanRunAway(0), "free == pin hate 0") && ok;

    // Dense compose: free CanRunAway matches GetHateTier(hate) >= 3, inline, pin, and hate >= 45.
    for (int32 hate = -20; hate <= 120; ++hate)
    {
        const bool got  = CanRunAway(hate);
        const bool want = GetHateTier(hate) >= 3;
        ok = expect(got == want, "compose range free == GetHateTier>=3") && ok;
        ok = expect(got == inlineCanRunAway(hate), "compose range free == inline") && ok;
        ok = expect(got == pinCanRunAway(hate), "compose range free == pin") && ok;
        ok = expect(got == (hate >= kHateTier3Min), "compose range free == hate>=45") && ok;
    }

    // Production SPAWN path semantics:
    // Eligible (tier3) → may set local var RunAway = 1.
    // Blocked (tier < 3) → no RunAway flag.
    ok = expect(CanRunAway(kHateTier3Min), "SPAWN eligible HateTier3Min → continue") && ok;
    ok = expect(!CanRunAway(kHateTier3Min - 1), "SPAWN blocked HateTier3Min-1") && ok;
    ok = expect(!CanRunAway(kHateTier2Min), "SPAWN blocked HateTier2Min") && ok;
    ok = expect(!CanRunAway(kHateTier1Min), "SPAWN blocked HateTier1Min") && ok;

    return ok;
}
