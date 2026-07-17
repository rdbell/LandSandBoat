#include "test_recast_update_charge_time_3360.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldUpdateChargeTime 3360 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Load chargeTime update gate for dual-wire cross-check (slice 3360):
//   chargeTimeNonzero
auto inlineShouldUpdateChargeTime(const bool chargeTimeNonzero) -> bool
{
    return chargeTimeNonzero;
}

// Pin form of the same formula (free == inline == pin).
auto pinShouldUpdateChargeTime(const bool chargeTimeNonzero) -> bool
{
    return chargeTimeNonzero;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldUpdateChargeTime
// (chargeTime != 0 update on existing Load entry; slice 3360 of residual
// 3104 / pure 1370). Residual suite: test_recast_update_charge_time_3104.
auto runRecastUpdateChargeTime3360SelfTests() -> bool
{
    using recasthelpers::ShouldUpdateChargeTime;

    bool ok = true;

    // Residual 3104 / 1370 pins still hold under dual-wire.
    ok = expect(ShouldUpdateChargeTime(true), "residual update on nonzero chargeTime") && ok;
    ok = expect(!ShouldUpdateChargeTime(false), "residual no update when chargeTime zero") && ok;

    const struct
    {
        bool        chargeTimeNonzero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "nonzero chargeTime updates" },
        { false, false, "zero chargeTime keeps existing" },

        // Residual 3104 / 1370 pins restated.
        { true, true, "residual 3104/1370 update charge" },
        { false, false, "residual 3104/1370 no update charge" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldUpdateChargeTime(c.chargeTimeNonzero);
        const bool pinF    = pinShouldUpdateChargeTime(c.chargeTimeNonzero);
        const bool inlineF = inlineShouldUpdateChargeTime(c.chargeTimeNonzero);
        const bool wantPin = c.chargeTimeNonzero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == pinF, "ShouldUpdateChargeTime dual-wire == pin formula") && ok;
        ok = expect(got == inlineF, "ShouldUpdateChargeTime dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldUpdateChargeTime == pin formula chargeTimeNonzero") && ok;
    }

    // Pin composition: identity only.
    ok = expect(ShouldUpdateChargeTime(true), "nonzero chargeTime must update") && ok;
    ok = expect(!ShouldUpdateChargeTime(false), "zero chargeTime must not update") && ok;

    // Dense compose: both bool inputs (full domain 2^1); free == inline == pin.
    for (const bool chargeTimeNonzero : { false, true })
    {
        const bool got  = ShouldUpdateChargeTime(chargeTimeNonzero);
        const bool want = chargeTimeNonzero;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldUpdateChargeTime(chargeTimeNonzero), "compose free == inline") && ok;
        ok              = expect(got == pinShouldUpdateChargeTime(chargeTimeNonzero), "compose free == pin") && ok;
    }

    // Host-style inject poles: chargeTime != 0s / zero as bools.
    // (Live container Load overwrite is residual 1370 / recast_container tests.)
    for (const bool chargeNonzero : { true, false })
    {
        const bool inject = chargeNonzero; // mirrors chargeTime != 0s
        ok                = expect(ShouldUpdateChargeTime(inject) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldUpdateChargeTime(inject) == inlineShouldUpdateChargeTime(inject),
                    "host inject free == inline") &&
             ok;
        ok = expect(ShouldUpdateChargeTime(inject) == pinShouldUpdateChargeTime(inject),
                    "host inject free == pin") &&
             ok;
    }

    return ok;
}
