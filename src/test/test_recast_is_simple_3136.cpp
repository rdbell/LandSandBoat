#include "test_recast_is_simple_3136.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast IsSimpleRecast 3136 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Load simple-recast gate for dual-wire cross-check (slice 3136):
//   chargeTimeIsZero
auto inlineIsSimpleRecast(const bool chargeTimeIsZero) -> bool
{
    return chargeTimeIsZero;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::IsSimpleRecast
// (chargeTime == 0 simple full-replace on existing Load entry; slice 3136).
auto runRecastIsSimple3136SelfTests() -> bool
{
    using recasthelpers::IsSimpleRecast;

    bool ok = true;

    const struct
    {
        bool        chargeTimeIsZero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "zero chargeTime is simple" },
        { false, false, "nonzero chargeTime is charged" },

        // Residual 1370 pins.
        { true, true, "residual 1370 simple" },
        { false, false, "residual 1370 not simple" },
    };

    for (const auto& c : cases)
    {
        const bool got     = IsSimpleRecast(c.chargeTimeIsZero);
        const bool inlineF = inlineIsSimpleRecast(c.chargeTimeIsZero);
        const bool wantPin = c.chargeTimeIsZero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "IsSimpleRecast dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "IsSimpleRecast == pin formula chargeTimeIsZero") && ok;
    }

    // Pin composition: identity only.
    ok = expect(IsSimpleRecast(true), "zero chargeTime must be simple") && ok;
    ok = expect(!IsSimpleRecast(false), "nonzero chargeTime must not be simple") && ok;

    // Dense compose: both bool inputs (full domain 2^1).
    for (const bool chargeTimeIsZero : { false, true })
    {
        const bool got  = IsSimpleRecast(chargeTimeIsZero);
        const bool want = chargeTimeIsZero;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineIsSimpleRecast(chargeTimeIsZero), "compose free == inline") && ok;
    }

    // Host-style inject poles: chargeTime == 0s / nonzero as bools.
    // (Live container Load simple vs charged is residual 1370 / recast_container tests.)
    for (const bool chargeZero : { true, false })
    {
        const bool inject = chargeZero; // mirrors recast->chargeTime == 0s
        ok                = expect(IsSimpleRecast(inject) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(IsSimpleRecast(inject) == inlineIsSimpleRecast(inject),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
