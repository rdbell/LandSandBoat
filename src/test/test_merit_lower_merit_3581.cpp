#include "test_merit_lower_merit_3581.h"

#include "map/merit_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "merit ShouldLowerMerit 3581 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline LowerMerit count-decrement formula for dual-wire cross-check
// (slice 3581 dedicated expand residual 3054; prior dedicated 3537 / 3478):
//   count > 0
// Direct return form matching production free function / capacity.
auto inlineShouldLowerMerit(const uint8 count) -> bool
{
    return count > 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3478).
// Direct return only — same formula as production ShouldLowerMerit.
// Retained under dedicated expand residual 3581.
auto pinShouldLowerMerit3478(const uint8 count) -> bool
{
    return count > 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3537).
// Direct return only — same formula as production ShouldLowerMerit.
// Retained under dedicated expand residual 3581.
auto pinShouldLowerMerit3537(const uint8 count) -> bool
{
    return count > 0;
}

// Compact dual-wire pin matching free function / capacity body (slice 3581).
// Direct return only — same formula as production ShouldLowerMerit.
auto pinShouldLowerMerit3581(const uint8 count) -> bool
{
    return count > 0;
}

} // namespace

// Pure dual-wire expansion for meritshelpers::ShouldLowerMerit
// (count > 0 LowerMerit count-decrement gate; OmegaXI internal/merit; slice 3581
// dedicated expand residual 3054; prior dedicated 3537 / 3478). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == pin3537 (direct return formula)
//   - residual 3054 / 2810 / prior dedicated 3537 / 3478 poles still hold
//   - residual poles (0, 1, n) + dense interior
auto runMeritLowerMerit3581SelfTests() -> bool
{
    using meritshelpers::PlanLowerMerit;
    using meritshelpers::ShouldLowerMerit;

    bool ok = true;

    // Residual 3054 / 2810 / prior dedicated 3537 / 3478 pins still hold.
    ok = expect(ShouldLowerMerit(1), "residual: count 1 should lower") && ok;
    ok = expect(ShouldLowerMerit(15), "residual: count 15 should lower") && ok;
    ok = expect(ShouldLowerMerit(255), "residual: count max uint8 should lower") && ok;
    ok = expect(!ShouldLowerMerit(0), "residual: count 0 should block") && ok;

    const struct
    {
        uint8       count;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 3054 / 2810 / prior 3537 / 3478 classic poles (0, 1, n).
        { 0, false, "residual count 0 blocks lower" },
        { 1, true, "residual count 1 admits lower" },
        { 15, true, "residual count 15 admits lower" },
        { 255, true, "residual count max uint8 admits lower" },

        // Dense interior / edge poles.
        { 2, true, "count 2 admits lower" },
        { 5, true, "count 5 admits lower" },
        { 128, true, "count mid admits lower" },
        { 254, true, "count 254 admits lower" },

        // Host inject path poles (CMeritPoints::LowerMerit → PlanLowerMerit).
        { 0, false, "host absent-or-zero count blocks lower" },
        { 1, true, "host single upgrade admits lower" },
        { 5, true, "host mid upgrades admit lower" },
        { 15, true, "host upgrade-max style count admits lower" },
        { 255, true, "host uint8 max admits lower" },
    };

    for (const auto& p : poles)
    {
        const bool got           = ShouldLowerMerit(p.count);
        const bool inlineF       = inlineShouldLowerMerit(p.count);
        const bool pin           = pinShouldLowerMerit3581(p.count);
        const bool priorPin3537  = pinShouldLowerMerit3537(p.count);
        const bool priorPin3478  = pinShouldLowerMerit3478(p.count);
        const bool wantPin       = p.count > 0;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "ShouldLowerMerit dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "ShouldLowerMerit == pin formula") && ok;
        ok = expect(got == priorPin3537, "ShouldLowerMerit == prior 3537 pin") && ok;
        ok = expect(got == priorPin3478, "ShouldLowerMerit == prior 3478 pin") && ok;
        ok = expect(got == wantPin, "ShouldLowerMerit == pin formula count > 0") && ok;
    }

    // Dense poles: free == inline == pin == pin3537 (direct return) across residual + interior.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 3 },
                               uint8{ 4 }, uint8{ 5 }, uint8{ 7 }, uint8{ 8 },
                               uint8{ 15 }, uint8{ 16 }, uint8{ 32 }, uint8{ 64 },
                               uint8{ 127 }, uint8{ 128 }, uint8{ 200 }, uint8{ 254 },
                               uint8{ 255 } })
    {
        const bool got          = ShouldLowerMerit(count);
        const bool inlineF      = inlineShouldLowerMerit(count);
        const bool pin          = pinShouldLowerMerit3581(count);
        const bool priorPin3537 = pinShouldLowerMerit3537(count);
        const bool priorPin3478 = pinShouldLowerMerit3478(count);
        const bool want         = count > 0;

        ok = expect(got == want, "dense free == pin formula") && ok;
        ok = expect(got == inlineF, "dense free == inline") && ok;
        ok = expect(got == pin, "dense free == pin") && ok;
        ok = expect(got == priorPin3537, "dense free == prior 3537 pin") && ok;
        ok = expect(got == priorPin3478, "dense free == prior 3478 pin") && ok;
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(!ShouldLowerMerit(0), "count 0 must block") && ok;
    ok = expect(ShouldLowerMerit(1), "count 1 must admit") && ok;
    ok = expect(ShouldLowerMerit(15), "count 15 must admit") && ok;
    ok = expect(ShouldLowerMerit(255), "count 255 must admit") && ok;

    // Residual independence (2810): PlanLowerMerit remains orthogonal compose
    // of meritPresent && ShouldLowerMerit(count).
    for (const bool present : { false, true })
    {
        for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 15 }, uint8{ 255 } })
        {
            const auto plan      = PlanLowerMerit(present, count);
            const bool wantApply = present && ShouldLowerMerit(count);
            ok                   = expect(plan.apply == wantApply, "PlanLowerMerit compose Apply") && ok;

            const bool got = ShouldLowerMerit(count);
            ok             = expect(got == inlineShouldLowerMerit(count),
                        "compose free == inline") &&
                 ok;
            ok = expect(got == pinShouldLowerMerit3581(count), "compose free == pin") && ok;
            ok = expect(got == pinShouldLowerMerit3537(count), "compose free == prior 3537 pin") && ok;
            ok = expect(got == pinShouldLowerMerit3478(count), "compose free == prior 3478 pin") && ok;
            ok = expect(got == (count > 0), "compose free == pin formula") && ok;
        }
    }

    // Explicit polarity under dedicated dual-wire: block only at zero.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 15 },
                               uint8{ 128 }, uint8{ 254 }, uint8{ 255 } })
    {
        const bool got = ShouldLowerMerit(count);
        const bool pin = pinShouldLowerMerit3581(count);
        ok             = expect(got == pin, "polarity free == pin") && ok;
        ok             = expect(!(count == 0 && got), "polarity: must not lower when count 0") && ok;
        ok             = expect(!(count > 0 && !got), "polarity: must lower when count nonzero") && ok;
    }

    // Prior dedicated 3537 independence: free still matches prior pin.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 15 },
                               uint8{ 128 }, uint8{ 254 }, uint8{ 255 } })
    {
        const bool got      = ShouldLowerMerit(count);
        const bool priorPin = pinShouldLowerMerit3537(count);
        ok                  = expect(got == priorPin, "prior 3537 independence") && ok;
    }

    // Prior dedicated 3478 independence: free still matches prior pin.
    for (const uint8 count : { uint8{ 0 }, uint8{ 1 }, uint8{ 2 }, uint8{ 15 },
                               uint8{ 128 }, uint8{ 254 }, uint8{ 255 } })
    {
        const bool got      = ShouldLowerMerit(count);
        const bool priorPin = pinShouldLowerMerit3478(count);
        ok                  = expect(got == priorPin, "prior 3478 independence") && ok;
    }

    return ok;
}
