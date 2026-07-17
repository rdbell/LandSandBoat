#include "test_aicontainer_can_change_state_3272.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanChangeState 3272 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::CanChangeState for dual-wire cross-check (dedicated 3272):
//   !hasCurrentState || currentCanChange
auto inlineCanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

// Compact dual-wire pin matching Go pinCanChangeState3272 / C++ capacity
// OR form (formula unchanged from 1189 / 2952). Direct OR (not if/else):
//   !hasCurrentState || currentCanChange
auto pinCanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

} // namespace

// Pure dual-wire expansion for aicontainerhelpers::CanChangeState
// (!hasCurrentState || currentCanChange; OmegaXI internal/aicontainer;
// dedicated slice 3272; residual expand 2952 / pure 1189).
//
// Coverage:
//   - free == inline == pin OR form
//   - residual poles: idle/active × canChange
//   - dense 2^2 free == inline == pin
//   - host-style inject + residual independence (2947 / 3222 left alone)
auto runAicontainerCanChangeState3272SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;

    bool ok = true;

    // Residual 1189 / 2952 pins still hold under dedicated dual-wire.
    ok = expect(CanChangeState(false, false), "residual: idle + currentCanChange false accepts") && ok;
    ok = expect(CanChangeState(false, true), "residual: idle + currentCanChange true accepts") && ok;
    ok = expect(!CanChangeState(true, false), "residual: active + !canChange rejects") && ok;
    ok = expect(CanChangeState(true, true), "residual: active + canChange accepts") && ok;

    // --- Core poles: free == inline == pin OR form ---
    const struct
    {
        bool        hasCurrentState;
        bool        currentCanChange;
        bool        want;
        const char* label;
    } cases[] = {
        // Pole: idle ignores currentCanChange.
        { false, false, true, "idle + currentCanChange false accepts" },
        { false, true, true, "idle + currentCanChange true accepts" },

        // Pole: active defers to currentCanChange.
        { true, false, false, "active + !canChange rejects" },
        { true, true, true, "active + canChange accepts" },

        // Residual 2952 / 1189 re-pins.
        { false, false, true, "residual 2952 idle ignores currentCanChange false" },
        { false, true, true, "residual 2952 idle ignores currentCanChange true" },
        { true, false, false, "residual 2952 active blocks" },
        { true, true, true, "residual 2952 active allows" },
        { false, false, true, "residual 1189 idle false inject" },
        { false, true, true, "residual 1189 idle true inject" },
        { true, false, false, "residual 1189 active block" },
        { true, true, true, "residual 1189 active allow" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool inlineF = inlineCanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool pinGot  = pinCanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool wantPin = !c.hasCurrentState || c.currentCanChange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "dual-wire free==OR pin form") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(CanChangeState(false, false) == pinCanChangeState(false, false), "free==pin idle false") && ok;
    ok = expect(CanChangeState(false, true) == pinCanChangeState(false, true), "free==pin idle true") && ok;
    ok = expect(CanChangeState(true, false) == pinCanChangeState(true, false), "free==pin active block") && ok;
    ok = expect(CanChangeState(true, true) == pinCanChangeState(true, true), "free==pin active allow") && ok;

    // Dense compose: full 2^2 boolean space free == inline == pin == OR form.
    for (const bool hasCurrent : { false, true })
    {
        for (const bool currentCanChange : { false, true })
        {
            const bool got     = CanChangeState(hasCurrent, currentCanChange);
            const bool inlineF = inlineCanChangeState(hasCurrent, currentCanChange);
            const bool pinGot  = pinCanChangeState(hasCurrent, currentCanChange);
            const bool want    = !hasCurrent || currentCanChange;
            ok                 = expect(got == inlineF, "compose free==inline") && ok;
            ok                 = expect(got == pinGot, "compose free==pin") && ok;
            ok                 = expect(got == want, "compose free==OR form") && ok;
        }
    }

    // Host-style inject poles: CAIContainer::CanChangeState injects presence + policy.
    const struct
    {
        bool        hasCurrentState;
        bool        currentCanChange;
        bool        wantAccept;
        const char* label;
    } hostCases[] = {
        { false, false, true, "GetCurrentState() == nullptr → accept path (idle)" },
        { false, true, true, "GetCurrentState() == nullptr (inject ignored) → accept" },
        { true, false, false, "current->CanChangeState() false → reject path" },
        { true, true, true, "current->CanChangeState() true → accept path" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = CanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool inlineF = inlineCanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool pinGot  = pinCanChangeState(c.hasCurrentState, c.currentCanChange);

        ok = expect(got == c.wantAccept, c.label) && ok;
        ok = expect(got == inlineF, "host compose free==inline") && ok;
        ok = expect(got == pinGot, "host compose free==pin") && ok;
    }

    // Production CAIContainer path semantics:
    // idle → accept; active block → reject; active allow → accept.
    ok = expect(CanChangeState(false, false) && pinCanChangeState(false, false),
                "CanChangeState idle → accept path") &&
         ok;
    ok = expect(!CanChangeState(true, false) && !pinCanChangeState(true, false),
                "CanChangeState active block → reject path") &&
         ok;
    ok = expect(CanChangeState(true, true) && pinCanChangeState(true, true),
                "CanChangeState active allow → accept path") &&
         ok;

    // Host-style inject poles for common state policies.
    // Magic / item / weaponskill / mobskill always-false CanChangeState.
    const bool stateBlocks = false;
    ok                     = expect(!CanChangeState(true, stateBlocks) && !pinCanChangeState(true, stateBlocks),
                "magic/item-like active block must reject") &&
         ok;
    // Attack always-true CanChangeState; ability after Complete.
    const bool stateAllows = true;
    ok                     = expect(CanChangeState(true, stateAllows) && pinCanChangeState(true, stateAllows),
                "attack/completed-ability active allow must accept") &&
         ok;

    // Residual independence (1189 / 2952 / 2947 / 3222 left alone):
    // CanChangeState is distinct from CanDispatch.
    ok = expect(CanChangeState(false, false) && CanChangeState(false, true),
                "idle CanChangeState residual still holds under dual-wire") &&
         ok;
    ok = expect(!CanChangeState(true, false) && CanChangeState(true, true),
                "active CanChangeState residual still holds under dual-wire") &&
         ok;
    ok = expect(!CanDispatch(false),
                "no controller must reject via CanDispatch (2947/3222 left alone)") &&
         ok;
    ok = expect(CanDispatch(true),
                "controller present must accept via CanDispatch (2947/3222 left alone)") &&
         ok;

    return ok;
}
