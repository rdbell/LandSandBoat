#include "test_attackround_create_daken_3045.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround ShouldCreateDakenAttack 3045 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CreateDakenAttack TYPE_PC gate for dual-wire cross-check (slice 3045):
//   isPC
auto inlineShouldCreateDakenAttack(const bool isPC) -> bool
{
    return isPC;
}

} // namespace

// Pure dual-wire expansion for attackroundhelpers::ShouldCreateDakenAttack
// (isPC identity; slice 3045).
auto runAttackroundCreateDaken3045SelfTests() -> bool
{
    using attackroundhelpers::ResolveCreateDakenAttackPlan;
    using attackroundhelpers::ShouldCreateDakenAttack;
    using attackroundhelpers::ShouldProcDakenThrow;

    bool ok = true;

    // Residual 1375 truth-table pins.
    ok = expect(ShouldCreateDakenAttack(true), "residual: PC admits daken path") && ok;
    ok = expect(!ShouldCreateDakenAttack(false), "residual: non-PC skips daken path") && ok;

    const struct
    {
        bool        isPC;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "PC admits CreateDakenAttack gate" },
        { false, false, "non-PC rejects CreateDakenAttack gate" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldCreateDakenAttack(c.isPC);
        const bool inlineF = inlineShouldCreateDakenAttack(c.isPC);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldCreateDakenAttack dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.isPC, "free == isPC (identity)") && ok;
    }

    // Pin composition: identity of isPC only.
    ok = expect(ShouldCreateDakenAttack(true), "isPC must admit") && ok;
    ok = expect(!ShouldCreateDakenAttack(false), "!isPC must reject") && ok;

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

        ok = expect(got == c.wantAdmit, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.isPC, "host compose free == isPC (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool isPC : { false, true })
    {
        const bool got  = ShouldCreateDakenAttack(isPC);
        const bool want = isPC;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldCreateDakenAttack(isPC), "compose free == inline") && ok;
    }

    // Residual independence (1375 / 2758): TYPE_PC gate is distinct from
    // shuriken/DAKEN throw proc and full ResolveCreateDakenAttackPlan.
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

    return ok;
}
