#include "test_enmity_zero_out_of_range_3485.h"

#include "map/enmity_container.h"
#include "map/enmity_zero_out_of_range_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldZeroEnmityOutOfRange 3485 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity out-of-range CE/VE zero gate for dual-wire cross-check
// (slice 3485 dedicated expand residual 2927; prior dedicated expand 3427 /
// 3333 / prior dedicated 3157):
//   !withinRange
// Direct return form matching production free function / capacity.
auto inlineShouldZeroEnmityOutOfRange3485(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching free function / capacity body (slice 3485).
// Direct return only — same formula as production ShouldZeroEnmityOutOfRange.
// Simple identity-not (negation of the host-injected withinRange bool).
auto pinShouldZeroEnmityOutOfRange3485(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3427 pin.
auto pinShouldZeroEnmityOutOfRange3427(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3333 pin.
auto pinShouldZeroEnmityOutOfRange3333(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated 3157 residual pin.
auto pinShouldZeroEnmityOutOfRange3157(const bool withinRange) -> bool
{
    return !withinRange;
}

} // namespace

// Pure dual-wire expansion for enmityrangehelpers::ShouldZeroEnmityOutOfRange
// (!IsWithinEnmityRange → CE=VE=0; OmegaXI internal/enmity;
// slice 3485 dedicated expand residual 2927; prior dedicated expand 3427 /
// 3333 / prior dedicated 3157).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return !withinRange identity-not)
//   - residual poles: out of range zeros / within range keeps
//   - dense full 2^1 boolean domain
//   - residual 1357 / 2927 / prior dedicated 3157 / prior expand 3333 /
//     prior expand 3427 pins still hold
auto runEnmityZeroOutOfRange3485SelfTests() -> bool
{
    using enmityrangehelpers::ShouldZeroEnmityOutOfRange;

    bool ok = true;

    // Residual 1357 / 2927 / prior dedicated 3157 / prior expand 3333 /
    // prior expand 3427 pins still hold.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "residual !withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "residual withinRange must not zero") && ok;

    // Residual poles: free == inline == pin (direct return).
    const struct
    {
        bool        withinRange;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 2927 classic dual poles.
        { false, true, "residual out of range zeros CE/VE" },
        { true, false, "residual within range keeps CE/VE" },

        // Residual 2927 re-pins.
        { false, true, "residual 2927 zero oor" },
        { true, false, "residual 2927 in range no zero" },

        // Residual 1357 re-pins.
        { false, true, "residual 1357 zero oor" },
        { true, false, "residual 1357 in range no zero" },

        // Prior dedicated 3157 poles.
        { false, true, "prior 3157 out of range zeros CE/VE" },
        { true, false, "prior 3157 within range keeps CE/VE" },
        { false, true, "prior 3157 residual 2927 zero oor" },
        { true, false, "prior 3157 residual 2927 in range no zero" },

        // Prior dedicated expand 3333 poles.
        { false, true, "prior 3333 out of range zeros CE/VE" },
        { true, false, "prior 3333 within range keeps CE/VE" },
        { false, true, "prior 3333 residual 2927 zero oor" },
        { true, false, "prior 3333 residual 2927 in range no zero" },

        // Prior dedicated expand 3427 poles.
        { false, true, "prior 3427 out of range zeros CE/VE" },
        { true, false, "prior 3427 within range keeps CE/VE" },
        { false, true, "prior 3427 residual 2927 zero oor" },
        { true, false, "prior 3427 residual 2927 in range no zero" },

        // Host inject path poles (UpdateEnmity range gate).
        { false, true, "host out of range: zero CE/VE" },
        { true, false, "host within range: keep CE/VE" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldZeroEnmityOutOfRange(p.withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange3485(p.withinRange);
        const bool pin     = pinShouldZeroEnmityOutOfRange3485(p.withinRange);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "ShouldZeroEnmityOutOfRange dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldZeroEnmityOutOfRange == pin formula") && ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3157(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior dedicated pin") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3333(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3333") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3427(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3427") &&
             ok;
        ok = expect(got == !p.withinRange, "ShouldZeroEnmityOutOfRange == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on withinRange only.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "!withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "withinRange must not zero") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(false) == pinShouldZeroEnmityOutOfRange3485(false), "free==pin false") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(true) == pinShouldZeroEnmityOutOfRange3485(true), "free==pin true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline.
    for (const bool withinRange : { false, true })
    {
        const bool got     = ShouldZeroEnmityOutOfRange(withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange3485(withinRange);
        const bool pin     = pinShouldZeroEnmityOutOfRange3485(withinRange);
        const bool want    = !withinRange;
        ok                 = expect(got == want, "dense free == pin formula") && ok;
        ok                 = expect(got == inlineF, "dense free == inline") && ok;
        ok                 = expect(got == pin, "dense free == pin") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3157(withinRange), "dense free == residual pin") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3333(withinRange), "dense free == prior expand pin 3333") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3427(withinRange), "dense free == prior expand pin 3427") && ok;
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
        ok                = expect(got == inlineShouldZeroEnmityOutOfRange3485(within), "compose free == inline") && ok;
        ok                = expect(got == pinShouldZeroEnmityOutOfRange3485(within), "compose free == pin") && ok;
    }

    // Residual 2927 / prior dedicated expand 3427 / 3333 / prior dedicated 3157 /
    // pure 1357 still hold under expand suite.
    ok = expect(ShouldZeroEnmityOutOfRange(false) && !ShouldZeroEnmityOutOfRange(true),
                "residual 2927/3427/3333/3157/1357 pins failed under 3485 suite") &&
         ok;

    return ok;
}
