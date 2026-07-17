#include "test_enmity_zero_out_of_range_2927.h"

#include "map/enmity_container.h"
#include "map/enmity_zero_out_of_range_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldZeroEnmityOutOfRange 2927 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity out-of-range CE/VE zero gate for dual-wire cross-check
// (slice 2927):
//   !withinRange
auto inlineShouldZeroEnmityOutOfRange(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace

// Pure dual-wire expansion for enmityrangehelpers::ShouldZeroEnmityOutOfRange
// (!IsWithinEnmityRange → CE=VE=0; slice 2927).
auto runEnmityZeroOutOfRange2927SelfTests() -> bool
{
    using enmityrangehelpers::ShouldZeroEnmityOutOfRange;

    bool ok = true;

    const struct
    {
        bool        withinRange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, true, "out of range zeros CE/VE" },
        { true, false, "within range keeps CE/VE" },

        // Residual 1357 pins.
        { false, true, "residual zero oor" },
        { true, false, "residual in range no zero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldZeroEnmityOutOfRange(c.withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange(c.withinRange);
        const bool wantPin = !c.withinRange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldZeroEnmityOutOfRange dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldZeroEnmityOutOfRange == pin formula !withinRange") && ok;
    }

    // Pin composition: negation identity only.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "!withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "withinRange must not zero") && ok;

    // Dense compose: both bool inputs (full domain).
    for (const bool withinRange : { false, true })
    {
        const bool got  = ShouldZeroEnmityOutOfRange(withinRange);
        const bool want = !withinRange;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldZeroEnmityOutOfRange(withinRange), "compose free == inline") && ok;
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
    }

    return ok;
}
