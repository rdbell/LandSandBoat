#include "test_recast_update_max_charges_3564.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldUpdateMaxCharges 3564 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Load maxCharges update gate for dual-wire cross-check (slice 3564):
//   maxChargesNonzero
auto inlineShouldUpdateMaxCharges(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// Pin form of the same formula (free == inline == pin). Positive pin:
// return maxChargesNonzero.
auto pinShouldUpdateMaxCharges(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// Dedicated 3564 pin (expand residual 3122). Positive: return maxChargesNonzero.
auto pinShouldUpdateMaxCharges3564(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// Prior dedicated 3519 pin (retained under 3564 expand residual 3122).
auto pinShouldUpdateMaxCharges3519(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// Prior dedicated 3448 pin (retained under 3564 expand residual 3122).
auto pinShouldUpdateMaxCharges3448(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// Prior dedicated 3391 pin (retained under 3564 expand residual 3122).
auto pinShouldUpdateMaxCharges3391(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldUpdateMaxCharges
// (maxCharges != 0 update on existing Load entry; slice 3564 dedicated expand
// residual 3122 / prior dedicated 3519 / 3448 / 3391 / pure 1370). Residual suites:
// test_recast_update_max_charges_3122, test_recast_update_max_charges_3391,
// test_recast_update_max_charges_3448, test_recast_update_max_charges_3519.
auto runRecastUpdateMaxCharges3564SelfTests() -> bool
{
    using recasthelpers::ShouldUpdateMaxCharges;

    bool ok = true;

    // Residual 3519 / 3448 / 3391 / 3122 / 1370 pins still hold under dual-wire.
    ok = expect(ShouldUpdateMaxCharges(true), "residual update on nonzero maxCharges") && ok;
    ok = expect(!ShouldUpdateMaxCharges(false), "residual no update when maxCharges zero") && ok;

    const struct
    {
        bool        maxChargesNonzero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "nonzero maxCharges updates" },
        { false, false, "zero maxCharges keeps existing" },

        // Residual 3519 / 3448 / 3391 / 3122 / 1370 pins restated.
        { true, true, "residual 3519/3448/3391/3122/1370 update max" },
        { false, false, "residual 3519/3448/3391/3122/1370 no update max" },
        // Residual 3519 re-pins.
        { true, true, "residual 3519 update max" },
        { false, false, "residual 3519 no update max" },
        // Residual 3448 re-pins.
        { true, true, "residual 3448 update max" },
        { false, false, "residual 3448 no update max" },
        // Residual 3391 re-pins.
        { true, true, "residual 3391 update max" },
        { false, false, "residual 3391 no update max" },
        // Residual 3122 re-pins.
        { true, true, "residual 3122 update max" },
        { false, false, "residual 3122 no update max" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpdateMaxCharges(c.maxChargesNonzero);
        const bool pinF    = pinShouldUpdateMaxCharges(c.maxChargesNonzero);
        const bool pin3564 = pinShouldUpdateMaxCharges3564(c.maxChargesNonzero);
        const bool pin3519 = pinShouldUpdateMaxCharges3519(c.maxChargesNonzero);
        const bool pin3448 = pinShouldUpdateMaxCharges3448(c.maxChargesNonzero);
        const bool pin3391 = pinShouldUpdateMaxCharges3391(c.maxChargesNonzero);
        const bool inlineF = inlineShouldUpdateMaxCharges(c.maxChargesNonzero);
        // Positive pin: return maxChargesNonzero.
        const bool wantPin = c.maxChargesNonzero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pinF, "ShouldUpdateMaxCharges dual-wire == pin formula") && ok;
        ok = expect(got == pin3564, "ShouldUpdateMaxCharges dual-wire == pin3564") && ok;
        ok = expect(got == pin3519, "ShouldUpdateMaxCharges dual-wire == residual pin3519") && ok;
        ok = expect(got == pin3448, "ShouldUpdateMaxCharges dual-wire == residual pin3448") && ok;
        ok = expect(got == pin3391, "ShouldUpdateMaxCharges dual-wire == residual pin3391") && ok;
        ok = expect(got == inlineF, "ShouldUpdateMaxCharges dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUpdateMaxCharges == pin formula maxChargesNonzero") && ok;
    }

    // Pin composition: identity only.
    ok = expect(ShouldUpdateMaxCharges(true), "nonzero maxCharges must update") && ok;
    ok = expect(!ShouldUpdateMaxCharges(false), "zero maxCharges must not update") && ok;
    ok = expect(ShouldUpdateMaxCharges(true) == pinShouldUpdateMaxCharges3564(true), "free == pin3564 true") && ok;
    ok = expect(ShouldUpdateMaxCharges(false) == pinShouldUpdateMaxCharges3564(false), "free == pin3564 false") && ok;
    ok = expect(ShouldUpdateMaxCharges(true) == pinShouldUpdateMaxCharges3519(true), "free == pin3519 true") && ok;
    ok = expect(ShouldUpdateMaxCharges(false) == pinShouldUpdateMaxCharges3519(false), "free == pin3519 false") && ok;

    // Dense compose: both bool inputs (full domain 2^1);
    // free == inline == pin == pin3519.
    for (const bool maxChargesNonzero : { false, true })
    {
        const bool got  = ShouldUpdateMaxCharges(maxChargesNonzero);
        const bool want = maxChargesNonzero;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldUpdateMaxCharges(maxChargesNonzero), "compose free == inline") && ok;
        ok              = expect(got == pinShouldUpdateMaxCharges(maxChargesNonzero), "compose free == pin") && ok;
        ok              = expect(got == pinShouldUpdateMaxCharges3564(maxChargesNonzero), "compose free == pin3564") && ok;
        ok              = expect(got == pinShouldUpdateMaxCharges3519(maxChargesNonzero), "compose free == pin3519") && ok;
        ok              = expect(got == pinShouldUpdateMaxCharges3448(maxChargesNonzero), "compose free == pin3448") && ok;
        ok              = expect(got == pinShouldUpdateMaxCharges3391(maxChargesNonzero), "compose free == pin3391") && ok;
    }

    // Host-style inject poles: maxCharges != 0 / zero as bools.
    // (Live container Load overwrite is residual 1370 / recast_container tests.)
    for (const bool maxNonzero : { true, false })
    {
        const bool inject = maxNonzero; // mirrors maxCharges != 0
        ok                = expect(ShouldUpdateMaxCharges(inject) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldUpdateMaxCharges(inject) == inlineShouldUpdateMaxCharges(inject),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldUpdateMaxCharges(inject) == pinShouldUpdateMaxCharges(inject),
                    "host inject free == pin") &&
             ok;
        ok = expect(ShouldUpdateMaxCharges(inject) == pinShouldUpdateMaxCharges3564(inject),
                    "host inject free == pin3564") &&
             ok;
        ok = expect(ShouldUpdateMaxCharges(inject) == pinShouldUpdateMaxCharges3519(inject),
                    "host inject free == pin3519") &&
             ok;
        ok = expect(ShouldUpdateMaxCharges(inject) == pinShouldUpdateMaxCharges3448(inject),
                    "host inject free == pin3448") &&
             ok;
        ok = expect(ShouldUpdateMaxCharges(inject) == pinShouldUpdateMaxCharges3391(inject),
                    "host inject free == pin3391") &&
             ok;
    }

    // Residual 3122 independence poles.
    ok = expect(ShouldUpdateMaxCharges(true) && !ShouldUpdateMaxCharges(false), "residual 3122 independence poles") && ok;
    // Residual 3391 independence: free still matches prior dedicated pin.
    ok = expect(ShouldUpdateMaxCharges(true) == pinShouldUpdateMaxCharges3391(true), "residual 3391 independence update") && ok;
    ok = expect(ShouldUpdateMaxCharges(false) == pinShouldUpdateMaxCharges3391(false), "residual 3391 independence no update") && ok;
    // Residual 3448 independence: free still matches prior dedicated pin.
    ok = expect(ShouldUpdateMaxCharges(true) == pinShouldUpdateMaxCharges3448(true), "residual 3448 independence update") && ok;
    ok = expect(ShouldUpdateMaxCharges(false) == pinShouldUpdateMaxCharges3448(false), "residual 3448 independence no update") && ok;
    // Residual 3519 independence: free still matches prior dedicated pin.
    ok = expect(ShouldUpdateMaxCharges(true) == pinShouldUpdateMaxCharges3519(true), "residual 3519 independence update") && ok;
    ok = expect(ShouldUpdateMaxCharges(false) == pinShouldUpdateMaxCharges3519(false), "residual 3519 independence no update") && ok;

    return ok;
}
