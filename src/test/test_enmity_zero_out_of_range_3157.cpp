#include "test_enmity_zero_out_of_range_3157.h"

#include "map/enmity_container.h"
#include "map/enmity_zero_out_of_range_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldZeroEnmityOutOfRange 3157 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity out-of-range CE/VE zero gate for dual-wire cross-check
// (residual 2927 / dedicated 3157):
//   !withinRange
auto inlineShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching Go pinShouldZeroEnmityOutOfRange3157:
//   !withinRange
// Simple identity-not (negation of the host-injected withinRange bool).
auto pinShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace

// Pure dual-wire expansion for enmityrangehelpers::ShouldZeroEnmityOutOfRange
// (!IsWithinEnmityRange → CE=VE=0; OmegaXI internal/enmity; slice 3157).
//
// Coverage:
//   - withinRange false → zero CE/VE / true → keep CE/VE
//   - free == inline == pin == !withinRange (identity-not)
//   - residual 1357 / 2927 pins still hold
auto runEnmityZeroOutOfRange3157SelfTests() -> bool
{
    using enmityrangehelpers::ShouldZeroEnmityOutOfRange;

    bool ok = true;

    // Residual 1357 / 2927 pins still hold under dual-wire.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "residual !withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "residual withinRange must not zero") && ok;

    // --- Eligible / blocked paths ---
    ok = expect(ShouldZeroEnmityOutOfRange(false), "out of range zeros CE/VE") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "within range keeps CE/VE") && ok;

    const struct
    {
        bool        withinRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "out of range zeros CE/VE" },
        { true, false, "within range keeps CE/VE" },

        // Residual 2927 re-pins.
        { false, true, "residual 2927 zero oor" },
        { true, false, "residual 2927 in range no zero" },

        // Residual 1357 re-pins.
        { false, true, "residual 1357 zero oor" },
        { true, false, "residual 1357 in range no zero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldZeroEnmityOutOfRange(c.withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange(c.withinRange);
        const bool pinGot  = pinShouldZeroEnmityOutOfRange(c.withinRange);
        const bool wantPin = !c.withinRange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldZeroEnmityOutOfRange dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinGot, "ShouldZeroEnmityOutOfRange dual-wire == pin") && ok;
        ok = expect(got == wantPin, "ShouldZeroEnmityOutOfRange == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on withinRange only.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "!withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "withinRange must not zero") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(false) == pinShouldZeroEnmityOutOfRange(false), "free==pin false") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(true) == pinShouldZeroEnmityOutOfRange(true), "free==pin true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool withinRange : { false, true })
    {
        const bool got  = ShouldZeroEnmityOutOfRange(withinRange);
        const bool want = !withinRange;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldZeroEnmityOutOfRange(withinRange), "compose free == inline") && ok;
        ok              = expect(got == pinShouldZeroEnmityOutOfRange(withinRange), "compose free == pin") && ok;
    }

    // Production enmitymath residual form matches capacity dual-wire.
    for (const bool withinRange : { false, true })
    {
        ok = expect(enmitymath::ShouldZeroEnmityOutOfRange(withinRange) ==
                        ShouldZeroEnmityOutOfRange(withinRange),
                    "capacity dual-wire == enmitymath residual") &&
             ok;
    }

    // Host-style compose: zero when pure range check fails.
    // IsWithinEnmityRangePure(sameZone, distanceIsWithin) → withinRange.
    const struct
    {
        bool        sameZone;
        bool        distanceIsWithin;
        bool        wantZero;
        const char* label;
    } composeCases[] = {
        { true, true, false, "same zone in distance: no zero" },
        { true, false, true, "same zone out of distance: zero" },
        { false, true, true, "cross zone (distance ignored): zero" },
        { false, false, true, "cross zone out of distance: zero" },
    };

    for (const auto& c : composeCases)
    {
        const bool within = enmitymath::IsWithinEnmityRangePure(c.sameZone, c.distanceIsWithin);
        const bool got    = ShouldZeroEnmityOutOfRange(within);
        ok                = expect(got == c.wantZero, c.label) && ok;
        ok                = expect(got == !within, "compose free == !withinRange pin") && ok;
        ok                = expect(got == inlineShouldZeroEnmityOutOfRange(within), "compose free == inline") && ok;
        ok                = expect(got == pinShouldZeroEnmityOutOfRange(within), "compose free == pin") && ok;
    }

    // Residual 2927 / 1357 still hold under dedicated suite.
    ok = expect(ShouldZeroEnmityOutOfRange(false) && !ShouldZeroEnmityOutOfRange(true),
                "residual 2927/1357 pins failed under 3157 suite") &&
         ok;

    return ok;
}
