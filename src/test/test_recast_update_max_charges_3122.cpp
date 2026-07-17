#include "test_recast_update_max_charges_3122.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldUpdateMaxCharges 3122 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Load maxCharges update gate for dual-wire cross-check (slice 3122):
//   maxChargesNonzero
auto inlineShouldUpdateMaxCharges(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldUpdateMaxCharges
// (maxCharges != 0 update on existing Load entry; slice 3122).
auto runRecastUpdateMaxCharges3122SelfTests() -> bool
{
    using recasthelpers::ShouldUpdateMaxCharges;

    bool ok = true;

    const struct
    {
        bool        maxChargesNonzero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "nonzero maxCharges updates" },
        { false, false, "zero maxCharges keeps existing" },

        // Residual 1370 pins.
        { true, true, "residual 1370 update max" },
        { false, false, "residual 1370 no update max" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpdateMaxCharges(c.maxChargesNonzero);
        const bool inlineF = inlineShouldUpdateMaxCharges(c.maxChargesNonzero);
        const bool wantPin = c.maxChargesNonzero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldUpdateMaxCharges dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUpdateMaxCharges == pin formula maxChargesNonzero") && ok;
    }

    // Pin composition: identity only.
    ok = expect(ShouldUpdateMaxCharges(true), "nonzero maxCharges must update") && ok;
    ok = expect(!ShouldUpdateMaxCharges(false), "zero maxCharges must not update") && ok;

    // Dense compose: both bool inputs (full domain 2^1).
    for (const bool maxChargesNonzero : { false, true })
    {
        const bool got  = ShouldUpdateMaxCharges(maxChargesNonzero);
        const bool want = maxChargesNonzero;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldUpdateMaxCharges(maxChargesNonzero), "compose free == inline") && ok;
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
    }

    return ok;
}
