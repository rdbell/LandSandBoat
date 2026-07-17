#include "test_enmity_zero_out_of_range_3859.h"

#include "map/enmity_container.h"
#include "map/enmity_zero_out_of_range_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "enmity ShouldZeroEnmityOutOfRange 3859 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateEnmity out-of-range CE/VE zero gate for dual-wire cross-check
// (slice 3859 dedicated expand residual 2927; prior dedicated expand 3814 /
// 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157):
//   !withinRange
// Direct return form matching production free function / capacity.
auto inlineShouldZeroEnmityOutOfRange3859(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching free function / capacity body (slice 3859).
// Direct return only — same formula as production ShouldZeroEnmityOutOfRange.
// Simple identity-not (negation of the host-injected withinRange bool).
// Positive form: free == inline == pin == pin3814.
auto pinShouldZeroEnmityOutOfRange3859(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3814 pin.
auto pinShouldZeroEnmityOutOfRange3814(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3769 pin.
auto pinShouldZeroEnmityOutOfRange3769(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3724 pin.
auto pinShouldZeroEnmityOutOfRange3724(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3679 pin.
auto pinShouldZeroEnmityOutOfRange3679(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3634 pin.
auto pinShouldZeroEnmityOutOfRange3634(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3589 pin.
auto pinShouldZeroEnmityOutOfRange3589(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3545 pin.
auto pinShouldZeroEnmityOutOfRange3545(const bool withinRange) -> bool
{
    return !withinRange;
}

// Compact dual-wire pin matching prior dedicated expand residual 3485 pin.
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
// slice 3859 dedicated expand residual 2927; prior dedicated expand 3814 /
// 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157).
// Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3814 (direct return !withinRange identity-not)
//   - residual poles: out of range zeros / within range keeps
//   - dense full 2^1 boolean domain
//   - residual 1357 / 2927 / prior dedicated 3157 / prior expand 3333 /
//     prior expand 3427 / prior expand 3485 / prior expand 3545 /
//     prior expand 3589 / prior expand 3634 / prior expand 3679 /
//     prior expand 3724 / prior expand 3769 / prior expand 3814 pins still hold
auto runEnmityZeroOutOfRange3859SelfTests() -> bool
{
    using enmityrangehelpers::ShouldZeroEnmityOutOfRange;

    bool ok = true;

    // Residual 1357 / 2927 / prior dedicated 3157 / prior expand 3333 /
    // prior expand 3427 / prior expand 3485 / prior expand 3545 /
    // prior expand 3589 / prior expand 3634 / prior expand 3679 /
    // prior expand 3724 / prior expand 3769 / prior expand 3814 pins still hold.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "residual !withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "residual withinRange must not zero") && ok;

    // Residual poles: free == inline == pin == pin3814 (direct return).
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

        // Prior dedicated expand 3485 poles.
        { false, true, "prior 3485 out of range zeros CE/VE" },
        { true, false, "prior 3485 within range keeps CE/VE" },
        { false, true, "prior 3485 residual 2927 zero oor" },
        { true, false, "prior 3485 residual 2927 in range no zero" },

        // Prior dedicated expand 3545 poles.
        { false, true, "prior 3545 out of range zeros CE/VE" },
        { true, false, "prior 3545 within range keeps CE/VE" },
        { false, true, "prior 3545 residual 2927 zero oor" },
        { true, false, "prior 3545 residual 2927 in range no zero" },

        // Prior dedicated expand 3589 poles.
        { false, true, "prior 3589 out of range zeros CE/VE" },
        { true, false, "prior 3589 within range keeps CE/VE" },
        { false, true, "prior 3589 residual 2927 zero oor" },
        { true, false, "prior 3589 residual 2927 in range no zero" },

        // Prior dedicated expand 3634 poles.
        { false, true, "prior 3634 out of range zeros CE/VE" },
        { true, false, "prior 3634 within range keeps CE/VE" },
        { false, true, "prior 3634 residual 2927 zero oor" },
        { true, false, "prior 3634 residual 2927 in range no zero" },

        // Prior dedicated expand 3679 poles.
        { false, true, "prior 3679 out of range zeros CE/VE" },
        { true, false, "prior 3679 within range keeps CE/VE" },
        { false, true, "prior 3679 residual 2927 zero oor" },
        { true, false, "prior 3679 residual 2927 in range no zero" },

        // Prior dedicated expand 3724 poles.
        { false, true, "prior 3724 out of range zeros CE/VE" },
        { true, false, "prior 3724 within range keeps CE/VE" },
        { false, true, "prior 3724 residual 2927 zero oor" },
        { true, false, "prior 3724 residual 2927 in range no zero" },

        // Prior dedicated expand 3769 poles.
        { false, true, "prior 3769 out of range zeros CE/VE" },
        { true, false, "prior 3769 within range keeps CE/VE" },
        { false, true, "prior 3769 residual 2927 zero oor" },
        { true, false, "prior 3769 residual 2927 in range no zero" },

        // Prior dedicated expand 3814 poles (independence).
        { false, true, "prior 3814 out of range zeros CE/VE" },
        { true, false, "prior 3814 within range keeps CE/VE" },
        { false, true, "prior 3814 residual 2927 zero oor" },
        { true, false, "prior 3814 residual 2927 in range no zero" },

        // Host inject path poles (UpdateEnmity range gate).
        { false, true, "host out of range: zero CE/VE" },
        { true, false, "host within range: keep CE/VE" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldZeroEnmityOutOfRange(p.withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange3859(p.withinRange);
        const bool pin     = pinShouldZeroEnmityOutOfRange3859(p.withinRange);

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
        ok = expect(got == pinShouldZeroEnmityOutOfRange3485(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3485") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3545(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3545") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3589(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3589") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3634(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3634") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3679(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3679") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3724(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3724") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3769(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3769") &&
             ok;
        ok = expect(got == pinShouldZeroEnmityOutOfRange3814(p.withinRange),
                    "ShouldZeroEnmityOutOfRange == prior expand pin 3814") &&
             ok;
        ok = expect(got == !p.withinRange, "ShouldZeroEnmityOutOfRange == pin formula (identity-not)") && ok;
    }

    // Pin composition: free function is identity-not on withinRange only.
    // free == inline == pin == pin3814.
    ok = expect(ShouldZeroEnmityOutOfRange(false), "!withinRange must zero") && ok;
    ok = expect(!ShouldZeroEnmityOutOfRange(true), "withinRange must not zero") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(false) == pinShouldZeroEnmityOutOfRange3859(false), "free==pin false") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(true) == pinShouldZeroEnmityOutOfRange3859(true), "free==pin true") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(false) == pinShouldZeroEnmityOutOfRange3814(false), "free==pin3814 false") && ok;
    ok = expect(ShouldZeroEnmityOutOfRange(true) == pinShouldZeroEnmityOutOfRange3814(true), "free==pin3814 true") && ok;

    // Dense compose: full 2^1 boolean space — free == pin == inline == pin3814.
    for (const bool withinRange : { false, true })
    {
        const bool got     = ShouldZeroEnmityOutOfRange(withinRange);
        const bool inlineF = inlineShouldZeroEnmityOutOfRange3859(withinRange);
        const bool pin     = pinShouldZeroEnmityOutOfRange3859(withinRange);
        const bool want    = !withinRange;
        ok                 = expect(got == want, "dense free == pin formula") && ok;
        ok                 = expect(got == inlineF, "dense free == inline") && ok;
        ok                 = expect(got == pin, "dense free == pin") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3157(withinRange), "dense free == residual pin") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3333(withinRange), "dense free == prior expand pin 3333") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3427(withinRange), "dense free == prior expand pin 3427") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3485(withinRange), "dense free == prior expand pin 3485") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3545(withinRange), "dense free == prior expand pin 3545") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3589(withinRange), "dense free == prior expand pin 3589") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3634(withinRange), "dense free == prior expand pin 3634") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3679(withinRange), "dense free == prior expand pin 3679") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3724(withinRange), "dense free == prior expand pin 3724") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3769(withinRange), "dense free == prior expand pin 3769") && ok;
        ok                 = expect(got == pinShouldZeroEnmityOutOfRange3814(withinRange), "dense free == prior expand pin 3814") && ok;
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
        ok                = expect(got == inlineShouldZeroEnmityOutOfRange3859(within), "compose free == inline") && ok;
        ok                = expect(got == pinShouldZeroEnmityOutOfRange3859(within), "compose free == pin") && ok;
        ok                = expect(got == pinShouldZeroEnmityOutOfRange3814(within), "compose free == pin3814") && ok;
    }

    // Residual 2927 / prior dedicated expand 3814 / 3769 / 3724 / 3679 / 3634 / 3589 / 3545 / 3485 / 3427 / 3333 / prior dedicated 3157 /
    // pure 1357 still hold under expand suite.
    ok = expect(ShouldZeroEnmityOutOfRange(false) && !ShouldZeroEnmityOutOfRange(true),
                "residual 2927/3814/3769/3724/3679/3634/3589/3545/3485/3427/3333/3157/1357 pins failed under 3859 suite") &&
         ok;

    return ok;
}
