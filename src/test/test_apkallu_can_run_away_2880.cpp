#include "test_apkallu_can_run_away_2880.h"

#include "map/apkallu_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "apkallu can run away 2880 self-test failed: " << label << '\n';
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

} // namespace

// Pure dual-wire expansion for apkalluhelpers::CanRunAway / GetHateTier
// (xi.apkallu.canRunAway after zone hate inject; slice 2880).
auto runApkalluCanRunAway2880SelfTests() -> bool
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

    // GetHateTier boundary table (residual 0925 pins).
    const struct
    {
        int32       hate;
        int32       wantTier;
        const char* label;
    } tierCases[] = {
        { -10, 0, "negative → tier 0" },
        { 0, 0, "0 → tier 0" },
        { 4, 0, "4 → tier 0" },
        { 5, 1, "5 → tier 1" },
        { 24, 1, "24 → tier 1" },
        { 25, 2, "25 → tier 2" },
        { 44, 2, "44 → tier 2" },
        { 45, 3, "45 → tier 3" },
        { 100, 3, "100 → tier 3" },
        { 999, 3, "999 → tier 3" },
    };

    for (const auto& c : tierCases)
    {
        const int32 got    = GetHateTier(c.hate);
        const int32 inlineT = inlineGetHateTier(c.hate);
        ok = expect(got == c.wantTier, c.label) && ok;
        ok = expect(got == inlineT, "GetHateTier dual-wire == inline") && ok;
    }

    // CanRunAway table: true only at tier >= 3 (hate >= 45).
    const struct
    {
        int32       hate;
        bool        want;
        const char* label;
    } runCases[] = {
        { -10, false, "negative cannot run" },
        { 0, false, "hate 0 cannot run" },
        { 4, false, "tier0 edge cannot run" },
        { 5, false, "tier1 min cannot run" },
        { 24, false, "tier1 max cannot run" },
        { 25, false, "tier2 min cannot run" },
        { 44, false, "tier2 max / residual 44 cannot run" },
        { 45, true, "tier3 min / residual 45 can run" },
        { 46, true, "tier3 interior can run" },
        { 100, true, "hate max can run" },
        { 999, true, "over-max hate can run" },
    };

    for (const auto& c : runCases)
    {
        const bool got     = CanRunAway(c.hate);
        const bool inlineF = inlineCanRunAway(c.hate);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRunAway dual-wire free == inline") && ok;
        ok = expect(got == (GetHateTier(c.hate) >= 3), "CanRunAway == GetHateTier>=3") && ok;
        ok = expect(got == (c.hate >= 45), "CanRunAway == hate>=45 (tier3 bound)") && ok;
    }

    // Residual 0925 CanRunAway pins.
    ok = expect(!CanRunAway(44), "residual CanRunAway(44)") && ok;
    ok = expect(CanRunAway(45), "residual CanRunAway(45)") && ok;
    ok = expect(CanRunAway(100), "residual CanRunAway(100)") && ok;

    // Dense compose: free CanRunAway matches GetHateTier(hate) >= 3 and hate >= 45.
    for (int32 hate = -20; hate <= 120; ++hate)
    {
        const bool got  = CanRunAway(hate);
        const bool want = GetHateTier(hate) >= 3;
        ok = expect(got == want, "compose range free == GetHateTier>=3") && ok;
        ok = expect(got == inlineCanRunAway(hate), "compose range free == inline") && ok;
        ok = expect(got == (hate >= 45), "compose range free == hate>=45") && ok;
    }

    return ok;
}
