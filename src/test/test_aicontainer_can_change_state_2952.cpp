#include "test_aicontainer_can_change_state_2952.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanChangeState 2952 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::CanChangeState for dual-wire cross-check (slice 2952):
//   !hasCurrentState || currentCanChange
auto inlineCanChangeState(const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return !hasCurrentState || currentCanChange;
}

} // namespace

// Pure dual-wire expansion for aicontainerhelpers::CanChangeState
// (current-state change gate; slice 2952).
auto runAicontainerCanChangeState2952SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;

    bool ok = true;

    const struct
    {
        bool        hasCurrentState;
        bool        currentCanChange;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, true, "idle + currentCanChange false accepts" },
        { false, true, true, "idle + currentCanChange true accepts" },
        { true, false, false, "active + !canChange rejects" },
        { true, true, true, "active + canChange accepts" },

        // Residual 1189 pins.
        { false, false, true, "residual idle ignores currentCanChange false" },
        { false, true, true, "residual idle ignores currentCanChange true" },
        { true, false, false, "residual active blocks" },
        { true, true, true, "residual active allows" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool inlineF = inlineCanChangeState(c.hasCurrentState, c.currentCanChange);
        const bool wantPin = !c.hasCurrentState || c.currentCanChange;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanChangeState dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanChangeState == pin formula !hasCurrent||currentCanChange") && ok;
    }

    // Pin composition: idle always true; active defers to currentCanChange.
    ok = expect(CanChangeState(false, false), "idle must accept even when currentCanChange false") && ok;
    ok = expect(CanChangeState(false, true), "idle must accept when currentCanChange true") && ok;
    ok = expect(!CanChangeState(true, false), "active + !canChange must reject") && ok;
    ok = expect(CanChangeState(true, true), "active + canChange must accept") && ok;

    // Dense compose: full 2^2 boolean space.
    for (const bool hasCurrent : { false, true })
    {
        for (const bool currentCanChange : { false, true })
        {
            const bool got  = CanChangeState(hasCurrent, currentCanChange);
            const bool want = !hasCurrent || currentCanChange;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineCanChangeState(hasCurrent, currentCanChange),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CAIContainer path semantics ---
    // Host injects:
    //   hasCurrentState  = GetCurrentState() != nullptr
    //   currentCanChange = GetCurrentState()->CanChangeState() (ignored when idle)
    // when true  → host may change state from external means
    // when false → host must not change state
    ok = expect(CanChangeState(false, false), "no current state → accept path (idle)") && ok;
    ok = expect(!CanChangeState(true, false), "current state blocks → reject path") && ok;
    ok = expect(CanChangeState(true, true), "current state allows → accept path") && ok;

    // Host-style inject poles for common state policies.
    // Magic / item / weaponskill / mobskill always-false CanChangeState.
    const bool stateBlocks = false;
    ok                     = expect(!CanChangeState(true, stateBlocks), "magic/item-like active block must reject") && ok;
    // Attack always-true CanChangeState; ability after Complete.
    const bool stateAllows = true;
    ok                     = expect(CanChangeState(true, stateAllows), "attack/completed-ability active allow must accept") && ok;
    ok                     = expect(CanChangeState(true, stateBlocks) == inlineCanChangeState(true, stateBlocks),
                "state blocks free == inline") &&
         ok;
    ok = expect(CanChangeState(true, stateAllows) == inlineCanChangeState(true, stateAllows),
                "state allows free == inline") &&
         ok;

    // Explicit dual-wire: free == pin formula for host-style poles.
    for (const bool hasCurrent : { false, true })
    {
        for (const bool canChange : { false, true })
        {
            ok = expect(CanChangeState(hasCurrent, canChange) == (!hasCurrent || canChange),
                        "host inject dual-wire identity") &&
                 ok;
            ok = expect(CanChangeState(hasCurrent, canChange) == inlineCanChangeState(hasCurrent, canChange),
                        "host inject free == inline") &&
                 ok;
        }
    }

    // Residual 1189 poles still hold under dual-wire.
    ok = expect(CanChangeState(false, false), "residual idle false inject") && ok;
    ok = expect(CanChangeState(false, true), "residual idle true inject") && ok;
    ok = expect(!CanChangeState(true, false), "residual active block") && ok;
    ok = expect(CanChangeState(true, true), "residual active allow") && ok;

    return ok;
}
