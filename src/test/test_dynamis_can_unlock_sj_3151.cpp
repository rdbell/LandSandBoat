#include "test_dynamis_can_unlock_sj_3151.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis CanUnlockSJ 3151 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua formula for dual-wire cross-check:
//   hasStatusEffect(SJ_RESTRICTION) and 1 or 0
auto inlineCanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

// Compact dual-wire pin matching Go pinCanUnlockSJ3151:
//   hasSJRestriction ? 1 : 0
auto pinCanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::CanUnlockSJ
// (somnialThresholdOnTrigger startEvent param; OmegaXI internal/dynamis;
// slice 3151).
//
// Coverage:
//   - has SJ_RESTRICTION → 1; no restriction → 0
//   - free == inline == pin == hasSJRestriction ? 1 : 0
//   - residual 1077 / 2921 pins still hold
//   - menuBits pairing 5 + canUnlockSJ*2
auto runDynamisCanUnlockSJ3151SelfTests() -> bool
{
    using dynamishelpers::CanUnlockSJ;

    bool ok = true;

    // Residual 1077 / 2921 pins still hold under dual-wire.
    ok = expect(CanUnlockSJ(true) == 1, "residual: has SJ_RESTRICTION → 1") && ok;
    ok = expect(CanUnlockSJ(false) == 0, "residual: no SJ_RESTRICTION → 0") && ok;

    // --- Core branches: free == inline == pin == formula ---
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
        // dedicated dual-wire poles
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
    }

    // Free == pin matching C++ across residual poles.
    ok = expect(CanUnlockSJ(true) == pinCanUnlockSJ(true), "free==pin hasSJ=true") && ok;
    ok = expect(CanUnlockSJ(false) == pinCanUnlockSJ(false), "free==pin hasSJ=false") && ok;

    // Branches distinct.
    ok = expect(CanUnlockSJ(true) != CanUnlockSJ(false), "branches distinct") && ok;
    ok = expect(CanUnlockSJ(true) == 1 && CanUnlockSJ(false) == 0, "compose both branches") && ok;

    // Dense 2^1 compose: free == inline == pin == formula.
    for (const bool hasSJ : { false, true })
    {
        const int got  = CanUnlockSJ(hasSJ);
        const int want = hasSJ ? 1 : 0;
        ok = expect(got == want, "compose hasSJ ? 1 : 0") && ok;
        ok = expect(got == inlineCanUnlockSJ(hasSJ), "compose free==inline") && ok;
        ok = expect(got == pinCanUnlockSJ(hasSJ), "compose free==pin") && ok;
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
