#include "test_aicontainer_despawn_dispatch_6305.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Despawn dispatch 6305 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Despawn branch selection:
//   hasController
auto inlineDespawnShouldDispatch(const bool hasController) -> bool
{
    return hasController;
}

// Compact dual-wire pin matching Go pinDespawnShouldDispatchController6305.
auto pinDespawnShouldDispatch(const bool hasController) -> bool
{
    return hasController;
}

} // namespace

// Pure dual-wire suite for public Despawn() controller-vs-Internal_Despawn
// branch (OmegaXI internal/aicontainer; slice 6305).
//
// Coverage:
//   - free == inline == pin identity hasController gate
//   - host-style inject poles (Controller->Despawn vs Internal_Despawn)
//   - residual independence (6291–6304 dual-wires / CanDispatch / CanChangeState)
auto runAicontainerDespawnDispatch6305SelfTests() -> bool
{
    using aicontainerhelpers::AcceptRaiseShouldInvoke;
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::DespawnShouldDispatchController;
    using aicontainerhelpers::InternalActionTargetAllowed;
    using aicontainerhelpers::InternalChangeTargetHasBattleEntity;
    using aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget;
    using aicontainerhelpers::InternalDespawnAllowed;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalDisengageHasBattleEntity;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;
    using aicontainerhelpers::InternalSynthAllowed;

    bool ok = true;

    // --- Branch: free == inline == pin (identity) ---
    ok = expect(DespawnShouldDispatchController(true), "has controller → Controller->Despawn()") && ok;
    ok = expect(!DespawnShouldDispatchController(false), "no controller → Internal_Despawn()") && ok;

    for (const bool hasController : { false, true })
    {
        const bool got     = DespawnShouldDispatchController(hasController);
        const bool inlineF = inlineDespawnShouldDispatch(hasController);
        const bool pinGot  = pinDespawnShouldDispatch(hasController);
        ok                 = expect(got == hasController, "free==identity") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
        ok                 = expect(got == pinGot, "free==pin") && ok;
    }

    // Host-style inject poles.
    const struct
    {
        bool        hasController;
        bool        wantDispatch;
        const char* label;
    } hostCases[] = {
        { true, true, "has controller → Controller->Despawn()" },
        { false, false, "no controller → Internal_Despawn()" },
    };

    for (const auto& c : hostCases)
    {
        const bool dispatch = DespawnShouldDispatchController(c.hasController);
        ok                  = expect(dispatch == c.wantDispatch, c.label) && ok;
    }

    // Else-path residual: Internal_Despawn admission (6300) still holds.
    ok = expect(!DespawnShouldDispatchController(false), "no controller must take Internal_Despawn path") && ok;
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "else-path residual: Internal_Despawn admission still holds") &&
         ok;

    // Sibling CanDispatch shares hasController inject formula.
    ok = expect(CanDispatch(true) == DespawnShouldDispatchController(true),
                "CanDispatch and DespawnShouldDispatchController agree when true") &&
         ok;
    ok = expect(CanDispatch(false) == DespawnShouldDispatchController(false),
                "CanDispatch and DespawnShouldDispatchController agree when false") &&
         ok;

    // Residual independence: 6291 admission still holds.
    ok = expect(InternalEngageForceAttackAllowed(true, false, false, false),
                "6291 residual: canChange alone still admits") &&
         ok;
    ok = expect(!InternalEngageForceAttackAllowed(false, true, false, false),
                "6291 residual: blocked incomplete still rejects") &&
         ok;
    ok = expect(InternalEngageShouldResumeInactive(true) && !InternalEngageShouldResumeInactive(false),
                "6291 residual: resume identity still holds") &&
         ok;

    // Residual independence: 6292 already-engaged path still holds.
    ok = expect(InternalEngageIsAlreadyEngagedPath(true, true) && !InternalEngageIsAlreadyEngagedPath(true, false),
                "6292 residual: already-engaged path still holds") &&
         ok;
    ok = expect(InternalEngageShouldRetarget(1, 2) && !InternalEngageShouldRetarget(3, 3),
                "6292 residual: retarget still holds") &&
         ok;

    // Residual independence: 6294/6296/6298 dual-wires still hold.
    ok = expect(InternalChangeTargetHasBattleEntity(true) && !InternalChangeTargetHasBattleEntity(false),
                "6294 residual: outer gate still holds") &&
         ok;
    ok = expect(InternalChangeTargetShouldSetBattleTarget(true, 1),
                "6294 residual: engaged still sets battle target") &&
         ok;
    ok = expect(!InternalChangeTargetShouldSetBattleTarget(false, 9),
                "6294 residual: not engaged nonzero still Engage") &&
         ok;
    ok = expect(InternalDisengageHasBattleEntity(true) && !InternalDisengageHasBattleEntity(false),
                "6296 residual: disengage outer gate still holds") &&
         ok;
    ok = expect(InternalDieHasBattleEntity(true) && !InternalDieHasBattleEntity(false),
                "6298 residual: die outer gate still holds") &&
         ok;

    // Residual independence: 6302/6303/6304 dual-wires still hold.
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;
    ok = expect(AcceptRaiseShouldInvoke(true) && !AcceptRaiseShouldInvoke(false),
                "6303 residual: accept raise admission still holds") &&
         ok;
    ok = expect(InternalSynthAllowed(true, false) && !InternalSynthAllowed(true, true),
                "6304 residual: synth admission still holds") &&
         ok;

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6305") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6305") &&
         ok;

    // Host evaluates branch first.
    ok = expect(DespawnShouldDispatchController(true), "controller present must dispatch") && ok;
    ok = expect(!DespawnShouldDispatchController(false), "controller absent must Internal_Despawn") && ok;

    return ok;
}
