#include "test_attackround_create_daken_3251.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround ShouldCreateDakenAttack 3251 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CreateDakenAttack TYPE_PC gate for dual-wire cross-check
// (dedicated slice 3251 expand residual 3045 / prior dedicated 3184):
//   isPC
auto inlineShouldCreateDakenAttack(const bool isPC) -> bool
{
    return isPC;
}

// Compact dual-wire pin matching Go pinShouldCreateDakenAttack3251 /
// C++ capacity direct-return form:
//   isPC
auto pinShouldCreateDakenAttack(const bool isPC) -> bool
{
    return isPC;
}

// Prior dedicated 3184 inline (independence cross-check):
//   isPC
auto inlineShouldCreateDakenAttack3184(const bool isPC) -> bool
{
    return isPC;
}

// Prior dedicated 3184 pin (independence cross-check):
//   isPC
auto pinShouldCreateDakenAttack3184(const bool isPC) -> bool
{
    return isPC;
}

} // namespace

// Pure dual-wire expansion for attackroundhelpers::ShouldCreateDakenAttack
// (isPC identity; dedicated slice 3251 expand residual 3045 / prior dedicated
// 3184 / pure 1375 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == isPC (pin direct return)
//   - residual 1375 / 3045 / prior 3184 pins still hold
//   - residual poles true/false (isPC admit / reject)
//   - both bool poles (isPC false/true) — dense 2¹
//   - sibling residual independence (ShouldProcDakenThrow / 2758 plan not re-expanded)
//   - prior 3184 independence (free == prior inline == prior pin)
auto runAttackroundCreateDaken3251SelfTests() -> bool
{
    using attackroundhelpers::ResolveCreateDakenAttackPlan;
    using attackroundhelpers::ShouldCreateDakenAttack;
    using attackroundhelpers::ShouldProcDakenThrow;

    bool ok = true;

    // Residual 1375 / 3045 / prior 3184 pins still hold under dual-wire.
    ok = expect(ShouldCreateDakenAttack(true), "residual 3045/1375/3184: PC admits daken path") && ok;
    ok = expect(!ShouldCreateDakenAttack(false), "residual 3045/1375/3184: non-PC skips daken path") && ok;

    // Residual poles true/false: free == inline == pin. Dense 2¹ over isPC.
    const struct
    {
        bool        isPC;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "residual pole PC admits CreateDakenAttack gate" },
        { false, false, "residual pole non-PC rejects CreateDakenAttack gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreateDakenAttack(c.isPC);
        const bool inlineF = inlineShouldCreateDakenAttack(c.isPC);
        const bool pinF    = pinShouldCreateDakenAttack(c.isPC);
        const bool wantPin = c.isPC;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreateDakenAttack dual-wire == inline LSB formula") && ok;
        ok = expect(got == pinF, "ShouldCreateDakenAttack dual-wire == pin formula") && ok;
        ok = expect(got == wantPin, "ShouldCreateDakenAttack == pin formula isPC (direct return)") && ok;
    }

    // Pin composition: identity of isPC only (direct return).
    ok = expect(ShouldCreateDakenAttack(true), "isPC must admit") && ok;
    ok = expect(!ShouldCreateDakenAttack(false), "!isPC must reject") && ok;

    // Explicit polarity: admit is exact identity of inject value.
    for (const bool isPC : { false, true })
    {
        const bool got  = ShouldCreateDakenAttack(isPC);
        const bool want = isPC;
        ok              = expect(got == want, "polarity: free == isPC") && ok;
        ok              = expect(!(got && !isPC), "polarity: never admit when !isPC") && ok;
        ok              = expect(!(!got && isPC), "polarity: always admit when isPC") && ok;
    }

    // Host path: CAttackRound::CreateDakenAttack TYPE_PC gate before ammo/proc.
    const struct
    {
        bool        isPC;
        bool        wantAdmit;
        const char* label;
    } hostCases[] = {
        { true, true, "objtype == TYPE_PC → enter ammo/proc work" },
        { false, false, "objtype != TYPE_PC → skip ammo/proc RNG path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldCreateDakenAttack(c.isPC);
        const bool inlineF = inlineShouldCreateDakenAttack(c.isPC);
        const bool pinF    = pinShouldCreateDakenAttack(c.isPC);

        ok = expect(got == c.wantAdmit, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinF, "host compose dual-wire free == pin") && ok;
        ok = expect(got == c.isPC, "host compose free == isPC (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools (2¹).
    for (const bool isPC : { false, true })
    {
        const bool got  = ShouldCreateDakenAttack(isPC);
        const bool want = isPC;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldCreateDakenAttack(isPC), "compose free == inline") &&
             ok;
        ok = expect(got == pinShouldCreateDakenAttack(isPC), "compose free == pin") && ok;
    }

    // Residual independence (1375 / 2758 / 3045 / 3184): TYPE_PC gate is distinct
    // from shuriken/DAKEN throw proc and full ResolveCreateDakenAttackPlan.
    // Sibling residuals not re-expanded under this dedicated suite.
    ok = expect(ShouldCreateDakenAttack(true), "PC must admit via ShouldCreateDakenAttack") && ok;
    ok = expect(ShouldProcDakenThrow(true, true), "shuriken+daken residual still procs under dual-wire") && ok;
    ok = expect(!ShouldProcDakenThrow(false, true), "non-shuriken must not throw via ShouldProcDakenThrow") && ok;
    ok = expect(!ResolveCreateDakenAttackPlan(false, true, true).addDakenThrow,
                "non-PC plan must stay empty even with shuriken+daken") &&
         ok;
    ok = expect(ResolveCreateDakenAttackPlan(true, true, true).addDakenThrow,
                "PC shuriken daken residual plan still adds throw") &&
         ok;
    ok = expect(ShouldCreateDakenAttack(true) && !ShouldProcDakenThrow(true, false),
                "PC gate admits while daken miss rejects throw residual") &&
         ok;

    // Prior dedicated 3184 independence: free == prior inline == prior pin.
    for (const bool isPC : { false, true })
    {
        const bool got         = ShouldCreateDakenAttack(isPC);
        const bool priorInline = inlineShouldCreateDakenAttack3184(isPC);
        const bool priorPin    = pinShouldCreateDakenAttack3184(isPC);
        ok                     = expect(got == priorInline, "prior 3184 independence free == prior inline") && ok;
        ok                     = expect(got == priorPin, "prior 3184 independence free == prior pin") && ok;
    }

    return ok;
}
