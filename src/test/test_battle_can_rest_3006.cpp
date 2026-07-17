#include "test_battle_can_rest_3006.h"

#include "map/battle_rest_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battle CanRest 3006 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CBattleEntity::CanRest pure gate for dual-wire cross-check (slice 3006):
//   !hasRegenDown && !hasNoRestFlag
auto inlineCanRest(const bool hasRegenDown, const bool hasNoRestFlag) -> bool
{
    return !hasRegenDown && !hasNoRestFlag;
}

} // namespace

// Pure dual-wire expansion for battleresthelpers::CanRest
// (!hasRegenDown && !hasNoRestFlag; slice 3006).
auto runBattleCanRest3006SelfTests() -> bool
{
    using battleresthelpers::CanRest;
    using battleresthelpers::ResolveResourcePercent;
    using battleresthelpers::ResolveRestPlan;

    bool ok = true;

    // Residual 1635 truth-table pins.
    ok = expect(CanRest(false, false), "residual: neither gate blocks → can rest") && ok;
    ok = expect(!CanRest(true, false), "residual: REGEN_DOWN blocks rest") && ok;
    ok = expect(!CanRest(false, true), "residual: NoRest flag blocks rest") && ok;

    const struct
    {
        bool        hasRegenDown;
        bool        hasNoRestFlag;
        bool        want;
        const char* label;
    } cases[] = {
        { false, false, true, "neither gate → can rest" },
        { true, false, false, "REGEN_DOWN only → cannot rest" },
        { false, true, false, "NoRest only → cannot rest" },
        { true, true, false, "both gates → cannot rest" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanRest(c.hasRegenDown, c.hasNoRestFlag);
        const bool inlineF = inlineCanRest(c.hasRegenDown, c.hasNoRestFlag);
        const bool wantPin = !c.hasRegenDown && !c.hasNoRestFlag;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanRest dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanRest == pin formula !hasRegenDown && !hasNoRestFlag") && ok;
    }

    // Pin composition: AND of negated injects.
    ok = expect(CanRest(false, false), "clear gates must allow rest") && ok;
    ok = expect(!CanRest(true, false), "REGEN_DOWN must block rest") && ok;
    ok = expect(!CanRest(false, true), "NoRest flag must block rest") && ok;
    ok = expect(!CanRest(true, true), "both gates must block rest") && ok;

    // Host path: CBattleEntity::CanRest injects
    // hasRegenDown = getMod(Mod::REGEN_DOWN) != 0,
    // hasNoRestFlag = HasStatusEffectByFlag(NoRest).
    const struct
    {
        bool        hasRegenDown;
        bool        hasNoRestFlag;
        bool        wantCanRest;
        const char* label;
    } hostCases[] = {
        { false, false, true, "no REGEN_DOWN, no NoRest → CanRest true" },
        { true, false, false, "REGEN_DOWN non-zero → CanRest false" },
        { false, true, false, "NoRest status flag → CanRest false" },
        { true, true, false, "both injects true → CanRest false" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = CanRest(c.hasRegenDown, c.hasNoRestFlag);
        const bool inlineF = inlineCanRest(c.hasRegenDown, c.hasNoRestFlag);

        ok = expect(got == c.wantCanRest, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == (!c.hasRegenDown && !c.hasNoRestFlag), "host compose free == pin formula") && ok;
    }

    // Dense compose: free == inline == pin formula for all bool poles.
    for (const bool hasRegenDown : { false, true })
    {
        for (const bool hasNoRestFlag : { false, true })
        {
            const bool got  = CanRest(hasRegenDown, hasNoRestFlag);
            const bool want = !hasRegenDown && !hasNoRestFlag;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanRest(hasRegenDown, hasNoRestFlag),
                        "compose free == inline") &&
                 ok;
        }
    }

    // Residual independence (1635): CanRest is distinct from resource-percent
    // and Rest plan pure halves that remain in the capacity suite.
    ok = expect(ResolveResourcePercent(50, 100) == 50,
                "resource-percent residual still holds under dual-wire") &&
         ok;
    {
        const auto p = ResolveRestPlan(50, 100, 50, 100, 0, 0.1f);
        ok           = expect(p.recoverResources && p.didRest,
                    "ResolveRestPlan residual still holds under dual-wire") &&
             ok;
    }
    // CanRest does not gate ResolveRestPlan (callers gate, matching LSB Rest).
    ok = expect(CanRest(false, false), "setup: clear CanRest should pass") && ok;
    ok = expect(!CanRest(true, false),
                "REGEN_DOWN still blocks via CanRest independently of Rest plan") &&
         ok;

    return ok;
}
