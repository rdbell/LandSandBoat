#include "test_merit_lower_3054.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit ShouldLowerMerit 3054 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline LowerMerit count-decrement formula for dual-wire cross-check (slice 3054):
//   count > 0
auto inlineShouldLowerMerit(const uint8 count) -> bool
{
    return count > 0;
}

} // namespace

// Pure dual-wire expansion for meritshelpers::ShouldLowerMerit
// (count > 0 LowerMerit count-decrement gate; slice 3054). Dense poles
// count 0 / 1 / 15 / 255; free == inline.
auto runMeritLower3054SelfTests() -> bool
{
    using meritshelpers::PlanLowerMerit;
    using meritshelpers::ShouldLowerMerit;

    bool ok = true;

    // Residual 2810 pins still hold under dual-wire.
    ok = expect(ShouldLowerMerit(1), "residual count 1 should lower") && ok;
    ok = expect(ShouldLowerMerit(15), "residual count 15 should lower") && ok;
    ok = expect(ShouldLowerMerit(255), "residual count max uint8 should lower") && ok;
    ok = expect(!ShouldLowerMerit(0), "residual count 0 should block") && ok;

    const struct
    {
        uint8       count;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic residual 2810 poles.
        { 0, false, "count 0 blocks lower" },
        { 1, true, "count 1 admits lower" },
        { 15, true, "count 15 admits lower" },
        { 255, true, "count max uint8 admits lower" },

        // Dense interior / edge poles.
        { 2, true, "count 2 admits lower" },
        { 5, true, "count 5 admits lower" },
        { 128, true, "count mid admits lower" },
        { 254, true, "count 254 admits lower" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldLowerMerit(c.count);
        const bool inlineF = inlineShouldLowerMerit(c.count);
        const bool wantPin = c.count > 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldLowerMerit dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldLowerMerit == pin formula count > 0") && ok;
    }

    // Pin composition: admit lower iff count > 0.
    ok = expect(!ShouldLowerMerit(0), "count 0 must block") && ok;
    ok = expect(ShouldLowerMerit(1), "count 1 must admit") && ok;
    ok = expect(ShouldLowerMerit(15), "count 15 must admit") && ok;
    ok = expect(ShouldLowerMerit(255), "count 255 must admit") && ok;

    // Dense compose: residual poles 0/1/15/255 + free == inline == pin.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 15 }, uint8{ 255 } })
    {
        const bool got  = ShouldLowerMerit(count);
        const bool want = count > 0;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldLowerMerit(count),
                    "compose free == inline") &&
             ok;
    }

    // Explicit polarity: block only at zero; every nonzero admits.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 15 },
                               uint8{ 128 }, uint8{ 254 }, uint8{ 255 } })
    {
        const bool got = ShouldLowerMerit(count);
        ok             = expect(!(count == 0 && got), "polarity: must not lower when count 0") && ok;
        ok             = expect(!(count > 0 && !got), "polarity: must lower when count nonzero") && ok;
        ok             = expect(got == (count > 0), "polarity: lower == count > 0") && ok;
    }

    // Host-style inject poles: PMerit->count as uint8 on LowerMerit.
    const struct
    {
        uint8       count;
        bool        wantLower;
        const char* label;
    } composeCases[] = {
        { 0, false, "zero count blocks lower" },
        { 1, true, "single upgrade admits lower" },
        { 5, true, "mid upgrades admit lower" },
        { 15, true, "upgrade-max style count admits lower" },
        { 255, true, "uint8 max admits lower" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldLowerMerit(c.count);
        ok             = expect(got == c.wantLower, c.label) && ok;
        ok             = expect(got == (c.count > 0), "compose free == pin formula") && ok;
        ok             = expect(got == inlineShouldLowerMerit(c.count),
                    "compose free == inline") &&
             ok;
    }

    // Production PlanLowerMerit path semantics:
    // count gate is only half — meritPresent is separate residual 2810 inject.
    ok = expect(!ShouldLowerMerit(0), "PlanLowerMerit present count 0 → count-half block") && ok;
    ok = expect(ShouldLowerMerit(1), "PlanLowerMerit present count 1 → count-half admit") && ok;

    // Sibling residual PlanLowerMerit is orthogonal composition:
    // apply = meritPresent && ShouldLowerMerit(count).
    for (const bool present : { false, true })
    {
        for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 15 }, uint8{ 255 } })
        {
            const auto plan      = PlanLowerMerit(present, count);
            const bool wantApply = present && ShouldLowerMerit(count);
            ok                   = expect(plan.apply == wantApply, "PlanLowerMerit compose Apply") && ok;
            ok                   = expect(ShouldLowerMerit(count) == (count > 0),
                        "count gate under PlanLowerMerit compose") &&
                 ok;
            ok = expect(ShouldLowerMerit(count) == inlineShouldLowerMerit(count),
                        "count gate free == inline under compose") &&
                 ok;
        }
    }

    // Explicit dual-wire poles: free == inline == pin across dense residual poles.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 15 },
                               uint8{ 128 }, uint8{ 254 }, uint8{ 255 } })
    {
        const bool got  = ShouldLowerMerit(count);
        const bool want = count > 0;
        ok              = expect(got == want, "host inject dual-wire pin") && ok;
        ok              = expect(got == inlineShouldLowerMerit(count),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
