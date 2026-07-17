#include "test_dynamis_can_unlock_sj_3672.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis CanUnlockSJ 3672 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua formula for dual-wire cross-check (slice 3672 expand residual 2921):
//   hasStatusEffect(SJ_RESTRICTION) and 1 or 0
auto inlineCanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Compact dual-wire pin matching Go pinCanUnlockSJ3672:
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated 3151 pin (retained under expand):
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ3151(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated expand residual 2921 pin 3422 (retained under expand):
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ3422(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated expand residual 2921 pin 3476 (retained under expand):
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ3476(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated expand residual 2921 pin 3538 (retained under expand):
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ3538(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated expand residual 2921 pin 3582 (retained under expand):
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ3582(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Prior dedicated expand residual 2921 pin 3627 (retained under expand):
//   hasSJRestriction ? 1 : 0
// free==inline==pin==pin3627 required for residual expand 3672.
auto pinCanUnlockSJ3627(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::CanUnlockSJ
// (somnialThresholdOnTrigger startEvent param; OmegaXI internal/dynamis;
// slice 3672 dedicated expand residual 2921; prior dedicated 3627 / 3582 / 3538 / 3476 / 3422 / 3151 retained).
//
// Coverage:
//   - has SJ_RESTRICTION → 1; no restriction → 0
//   - free == inline == pin == pin3627 == hasSJRestriction ? 1 : 0
//   - residual 1077 / 2921 pins still hold
//   - prior dedicated 3151 pin still holds
//   - prior dedicated expand residual 2921 pin 3422 still holds
//   - prior dedicated expand residual 2921 pin 3476 still holds
//   - prior dedicated expand residual 2921 pin 3538 still holds
//   - prior dedicated expand residual 2921 pin 3582 still holds
//   - prior dedicated expand residual 2921 pin 3627 still holds
//   - menuBits pairing 5 + canUnlockSJ*2
auto runDynamisCanUnlockSJ3672SelfTests() -> bool
{
    using dynamishelpers::CanUnlockSJ;

    bool ok = true;

    // Residual 1077 / 2921 pins still hold under dual-wire expand.
    ok = expect(CanUnlockSJ(true) == 1, "residual: has SJ_RESTRICTION → 1") && ok;
    ok = expect(CanUnlockSJ(false) == 0, "residual: no SJ_RESTRICTION → 0") && ok;

    // --- Core branches: free == inline == pin == pin3627 == formula ---
    // Positive form: has restriction → 1; no restriction → 0.
    const struct
    {
        bool        hasSJ;
        int         want;
        const char* label;
    } cases[] = {
        // residual 1077 / 2921 poles
        { true, 1, "residual has SJ_RESTRICTION → 1" },
        { false, 0, "residual no SJ_RESTRICTION → 0" },
        // prior dedicated 3151 poles
        { true, 1, "prior 3151 true → 1 (unlock available)" },
        { false, 0, "prior 3151 false → 0 (no unlock option)" },
        // prior dedicated expand residual 2921 poles (3422)
        { true, 1, "prior 3422 true → 1 (unlock available)" },
        { false, 0, "prior 3422 false → 0 (no unlock option)" },
        // prior dedicated expand residual 2921 poles (3476)
        { true, 1, "prior 3476 true → 1 (unlock available)" },
        { false, 0, "prior 3476 false → 0 (no unlock option)" },
        // prior dedicated expand residual 2921 poles (3538)
        { true, 1, "prior 3538 true → 1 (unlock available)" },
        { false, 0, "prior 3538 false → 0 (no unlock option)" },
        // prior dedicated expand residual 2921 poles (3582)
        { true, 1, "prior 3582 true → 1 (unlock available)" },
        { false, 0, "prior 3582 false → 0 (no unlock option)" },
        // prior dedicated expand residual 2921 poles (3627)
        { true, 1, "prior 3627 true → 1 (unlock available)" },
        { false, 0, "prior 3627 false → 0 (no unlock option)" },
        // dedicated dual-wire expand residual 2921 poles
        { true, 1, "true → 1 (unlock available)" },
        { false, 0, "false → 0 (no unlock option)" },
    };

    for (const auto& c : cases)
    {
        const int got     = CanUnlockSJ(c.hasSJ);
        const int inlineF = inlineCanUnlockSJ(c.hasSJ);
        const int pinGot  = pinCanUnlockSJ(c.hasSJ);
        // Positive formula pin: hasSJ → 1, else → 0.
        const int wantF   = c.hasSJ ? 1 : 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantF, "formula free==hasSJ ? 1 : 0") && ok;
        ok = expect(got == pinCanUnlockSJ3151(c.hasSJ), "prior 3151 pin still holds") && ok;
        ok = expect(got == pinCanUnlockSJ3422(c.hasSJ), "prior 3422 pin still holds") && ok;
        ok = expect(got == pinCanUnlockSJ3476(c.hasSJ), "prior 3476 pin still holds") && ok;
        ok = expect(got == pinCanUnlockSJ3538(c.hasSJ), "prior 3538 pin still holds") && ok;
        ok = expect(got == pinCanUnlockSJ3582(c.hasSJ), "prior 3582 pin still holds") && ok;
        ok = expect(got == pinCanUnlockSJ3627(c.hasSJ), "prior 3627 pin still holds") && ok;
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(CanUnlockSJ(true) == pinCanUnlockSJ(true), "free==pin hasSJ=true") && ok;
    ok = expect(CanUnlockSJ(false) == pinCanUnlockSJ(false), "free==pin hasSJ=false") && ok;
    // free == pin3627 (required dual-wire identity for residual expand).
    ok = expect(CanUnlockSJ(true) == pinCanUnlockSJ3627(true), "free==pin3627 hasSJ=true") && ok;
    ok = expect(CanUnlockSJ(false) == pinCanUnlockSJ3627(false), "free==pin3627 hasSJ=false") && ok;

    // Branches distinct.
    ok = expect(CanUnlockSJ(true) != CanUnlockSJ(false), "branches distinct") && ok;
    ok = expect(CanUnlockSJ(true) == 1 && CanUnlockSJ(false) == 0, "compose both branches") && ok;

    // Dense 2^1 compose: free == inline == pin == pin3627 == formula
    // (and prior 3627 / 3582 / 3538 / 3476 / 3422 / 3151 pins).
    for (const bool hasSJ : { false, true })
    {
        const int got  = CanUnlockSJ(hasSJ);
        const int want = hasSJ ? 1 : 0;
        ok = expect(got == want, "compose hasSJ ? 1 : 0") && ok;
        ok = expect(got == inlineCanUnlockSJ(hasSJ), "compose free==inline") && ok;
        ok = expect(got == pinCanUnlockSJ(hasSJ), "compose free==pin") && ok;
        ok = expect(got == pinCanUnlockSJ3151(hasSJ), "compose prior 3151 pin") && ok;
        ok = expect(got == pinCanUnlockSJ3422(hasSJ), "compose prior 3422 pin") && ok;
        ok = expect(got == pinCanUnlockSJ3476(hasSJ), "compose prior 3476 pin") && ok;
        ok = expect(got == pinCanUnlockSJ3538(hasSJ), "compose prior 3538 pin") && ok;
        ok = expect(got == pinCanUnlockSJ3582(hasSJ), "compose prior 3582 pin") && ok;
        ok = expect(got == pinCanUnlockSJ3627(hasSJ), "compose prior 3627 pin") && ok;
    }

    // menuBits pairing (somnial residual): 5 + canUnlockSJ*2
    ok = expect(5 + CanUnlockSJ(false) * 2 == 5, "menuBits no SJ = 5") && ok;
    ok = expect(5 + CanUnlockSJ(true) * 2 == 7, "menuBits with SJ = 7") && ok;

    // Production somnialThresholdOnTrigger path semantics:
    // free → startEvent param 2; host owns hasStatusEffect / startEvent.
    ok = expect(CanUnlockSJ(true) == 1, "production free-path: has SJ → 1") && ok;
    ok = expect(CanUnlockSJ(false) == 0, "production free-path: no SJ → 0") && ok;

    return ok;
}
