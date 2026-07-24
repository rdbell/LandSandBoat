#include "test_aicontainer_can_follow_path_6306.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer CanFollowPath 6306 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::CanFollowPath:
//   hasPathFind && (!hasCurrentState || currentCanChange)
auto inlineCanFollowPath(const bool hasPathFind, const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return hasPathFind && (!hasCurrentState || currentCanChange);
}

// Compact dual-wire pin matching Go pinCanFollowPath6306.
auto pinCanFollowPath(const bool hasPathFind, const bool hasCurrentState, const bool currentCanChange) -> bool
{
    return hasPathFind && aicontainerhelpers::CanChangeState(hasCurrentState, currentCanChange);
}

} // namespace

// Pure dual-wire suite for CanFollowPath (OmegaXI internal/aicontainer; slice 6306).
//
// Coverage:
//   - free == inline == pin (hasPathFind && CanChangeState(...))
//   - host-style inject poles
//   - residual independence (CanChangeState / CanDispatch / 6300–6305)
auto runAicontainerCanFollowPath6306SelfTests() -> bool
{
    using aicontainerhelpers::AcceptRaiseShouldInvoke;
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::CanFollowPath;
    using aicontainerhelpers::DespawnShouldDispatchController;
    using aicontainerhelpers::InternalActionTargetAllowed;
    using aicontainerhelpers::InternalDespawnAllowed;
    using aicontainerhelpers::InternalSynthAllowed;

    bool ok = true;

    ok = expect(!CanFollowPath(false, false, true), "no PathFind → false") && ok;
    ok = expect(CanFollowPath(true, false, false), "PathFind + idle → true") && ok;
    ok = expect(!CanFollowPath(true, true, false), "PathFind + active !canChange → false") && ok;
    ok = expect(CanFollowPath(true, true, true), "PathFind + active canChange → true") && ok;

    for (const bool hasPath : { false, true })
    {
        for (const bool hasCurrent : { false, true })
        {
            for (const bool canChange : { false, true })
            {
                const bool got     = CanFollowPath(hasPath, hasCurrent, canChange);
                const bool inlineF = inlineCanFollowPath(hasPath, hasCurrent, canChange);
                const bool pinGot  = pinCanFollowPath(hasPath, hasCurrent, canChange);
                const bool want    = hasPath && (!hasCurrent || canChange);
                ok                 = expect(got == want, "free==want") && ok;
                ok                 = expect(got == inlineF, "free==inline") && ok;
                ok                 = expect(got == pinGot, "free==pin") && ok;
            }
        }
    }

    // Composition residual: PathFind && CanChangeState.
    ok = expect(CanFollowPath(true, false, false) == (true && CanChangeState(false, false)),
                "PathFind idle composes CanChangeState") &&
         ok;
    ok = expect(CanFollowPath(true, true, false) == (true && CanChangeState(true, false)),
                "PathFind active block composes CanChangeState") &&
         ok;

    // Residual: CanChangeState / CanDispatch still hold.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6306") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6306") &&
         ok;

    // Residual independence: 6300–6305 dual-wires still hold.
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "6300 residual: despawn admission still holds") &&
         ok;
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;
    ok = expect(AcceptRaiseShouldInvoke(true) && !AcceptRaiseShouldInvoke(false),
                "6303 residual: accept raise still holds") &&
         ok;
    ok = expect(InternalSynthAllowed(true, false) && !InternalSynthAllowed(true, true),
                "6304 residual: synth admission still holds") &&
         ok;
    ok = expect(DespawnShouldDispatchController(true) && !DespawnShouldDispatchController(false),
                "6305 residual: despawn dispatch still holds") &&
         ok;

    return ok;
}
