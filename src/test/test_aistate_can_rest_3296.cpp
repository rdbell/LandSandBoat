#include "test_aistate_can_rest_3296.h"

#include "map/battle_rest_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aistate CanRest 3296 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CBattleEntity::CanRest pure gate for dual-wire cross-check
// (slice 3296 dedicated expand residual 3006):
//   !hasRegenDown && !hasNoRestFlag
// Direct return form matching production free function / capacity.
auto inlineCanRest(const bool hasRegenDown, const bool hasNoRestFlag) -> bool
{
    return !hasRegenDown && !hasNoRestFlag;
}

// Compact dual-wire pin matching free function / capacity body (slice 3296).
// Direct return only — same formula as production CanRest.
auto pinCanRest3296(const bool hasRegenDown, const bool hasNoRestFlag) -> bool
{
    return !hasRegenDown && !hasNoRestFlag;
}

} // namespace

// Pure dual-wire expansion for battleresthelpers::CanRest
// (!hasRegenDown && !hasNoRestFlag; OmegaXI internal/aistate; slice 3296
// dedicated expand residual 3006; prior dedicated 3253). Formula unchanged.
//
// Coverage:
//   - free == inline == pin (direct return formula)
//   - residual 3006 / 1635 / prior 3253 poles still hold
//   - residual poles + dense 2² bool space
auto runAistateCanRest3296SelfTests() -> bool
{
    using battleresthelpers::CanRest;
    using battleresthelpers::ResolveResourcePercent;
    using battleresthelpers::ResolveRestPlan;

    bool ok = true;

    // Residual 3006 / 1635 / prior 3253 pins still hold.
    ok = expect(CanRest(false, false), "residual: neither gate blocks → can rest") && ok;
    ok = expect(!CanRest(true, false), "residual: REGEN_DOWN blocks rest") && ok;
    ok = expect(!CanRest(false, true), "residual: NoRest flag blocks rest") && ok;
    ok = expect(!CanRest(true, true), "residual: both gates block rest") && ok;

    const struct
    {
        bool        hasRegenDown;
        bool        hasNoRestFlag;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 3006 / 1635 / prior 3253 classic dual poles.
        { false, false, true, "residual neither gate → can rest" },
        { true, false, false, "residual REGEN_DOWN only → cannot rest" },
        { false, true, false, "residual NoRest only → cannot rest" },
        { true, true, false, "residual both gates → cannot rest" },

        // Host inject path poles (CBattleEntity::CanRest).
        { false, false, true, "host no REGEN_DOWN, no NoRest → CanRest true" },
        { true, false, false, "host REGEN_DOWN non-zero → CanRest false" },
        { false, true, false, "host NoRest status flag → CanRest false" },
        { true, true, false, "host both injects true → CanRest false" },
    };

    for (const auto& p : poles)
    {
        const bool got     = CanRest(p.hasRegenDown, p.hasNoRestFlag);
        const bool inlineF = inlineCanRest(p.hasRegenDown, p.hasNoRestFlag);
        const bool pin     = pinCanRest3296(p.hasRegenDown, p.hasNoRestFlag);
        const bool wantPin = !p.hasRegenDown && !p.hasNoRestFlag;

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "CanRest dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "CanRest == pin formula") && ok;
        ok = expect(got == wantPin, "CanRest == pin formula !hasRegenDown && !hasNoRestFlag") && ok;
    }

    // Dense 2²: free == inline == pin (direct return) for all bool poles.
    for (const bool hasRegenDown : { false, true })
    {
        for (const bool hasNoRestFlag : { false, true })
        {
            const bool got     = CanRest(hasRegenDown, hasNoRestFlag);
            const bool inlineF = inlineCanRest(hasRegenDown, hasNoRestFlag);
            const bool pin     = pinCanRest3296(hasRegenDown, hasNoRestFlag);
            const bool want    = !hasRegenDown && !hasNoRestFlag;

            ok = expect(got == want, "dense free == pin formula") && ok;
            ok = expect(got == inlineF, "dense free == inline") && ok;
            ok = expect(got == pin, "dense free == pin") && ok;
        }
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(CanRest(false, false), "clear gates must allow rest") && ok;
    ok = expect(!CanRest(true, false), "REGEN_DOWN must block rest") && ok;
    ok = expect(!CanRest(false, true), "NoRest flag must block rest") && ok;
    ok = expect(!CanRest(true, true), "both gates must block rest") && ok;

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
