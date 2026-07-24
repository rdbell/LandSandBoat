#include "test_aicontainer_internal_despawn_6300.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Despawn 6300 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Despawn outer admission:
//   !IsCurrentState<CDespawnState>()
auto inlineDespawnAllowed(const bool isCurrentDespawnState) -> bool
{
    return !isCurrentDespawnState;
}

// Compact dual-wire pin matching Go pinInternalDespawnAllowed6300.
auto pinDespawnAllowed(const bool isCurrentDespawnState) -> bool
{
    return !isCurrentDespawnState;
}

} // namespace

// Pure dual-wire suite for Internal_Despawn not-already-despawning admission
// (OmegaXI internal/aicontainer; slice 6300).
//
// Coverage:
//   - free == inline == pin outer !isCurrentDespawnState gate
//   - host-style inject poles (admission → ForceChangeState<CDespawnState>+result vs false)
//   - residual independence (6291/6292/6294/6296/6298 dual-wires / CanDispatch / CanChangeState)
auto runAicontainerInternalDespawn6300SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalChangeTargetHasBattleEntity;
    using aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget;
    using aicontainerhelpers::InternalDespawnAllowed;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalDisengageHasBattleEntity;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;

    bool ok = true;

    // --- Outer admission: free == inline == pin (!isCurrentDespawnState) ---
    ok = expect(InternalDespawnAllowed(false), "not despawn state → ForceChangeState<CDespawnState> + return result") && ok;
    ok = expect(!InternalDespawnAllowed(true), "already CDespawnState → return false") && ok;

    for (const bool isDespawn : { false, true })
    {
        const bool got     = InternalDespawnAllowed(isDespawn);
        const bool inlineF = inlineDespawnAllowed(isDespawn);
        const bool pinGot  = pinDespawnAllowed(isDespawn);
        const bool want    = !isDespawn;
        ok                 = expect(got == want, "outer free==!isDespawn") && ok;
        ok                 = expect(got == inlineF, "outer free==inline") && ok;
        ok                 = expect(got == pinGot, "outer free==pin") && ok;
    }

    // Host-style inject poles: admission → ForceChangeState<CDespawnState>+result vs return false.
    const struct
    {
        bool        isCurrentDespawnState;
        bool        wantProceed;
        const char* label;
    } hostCases[] = {
        { false, true, "not despawn → ForceChangeState<CDespawnState> + return result" },
        { true, false, "already CDespawnState → return false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalDespawnAllowed(c.isCurrentDespawnState);
        ok                 = expect(proceed == c.wantProceed, c.label) && ok;
    }

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

    // Residual independence: 6294 ChangeTarget dual-wires still hold.
    ok = expect(InternalChangeTargetHasBattleEntity(true) && !InternalChangeTargetHasBattleEntity(false),
                "6294 residual: outer gate still holds") &&
         ok;
    ok = expect(InternalChangeTargetShouldSetBattleTarget(true, 1),
                "6294 residual: engaged still sets battle target") &&
         ok;
    ok = expect(!InternalChangeTargetShouldSetBattleTarget(false, 9),
                "6294 residual: not engaged nonzero still Engage") &&
         ok;

    // Residual independence: 6296 Disengage dual-wire still holds.
    ok = expect(InternalDisengageHasBattleEntity(true) && !InternalDisengageHasBattleEntity(false),
                "6296 residual: disengage outer gate still holds") &&
         ok;

    // Residual independence: 6298 Die dual-wire still holds.
    ok = expect(InternalDieHasBattleEntity(true) && !InternalDieHasBattleEntity(false),
                "6298 residual: die outer gate still holds") &&
         ok;

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6300") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6300") &&
         ok;

    // Host evaluates admission first.
    ok = expect(InternalDespawnAllowed(false), "not-already-despawning must pass admission") && ok;
    ok = expect(!InternalDespawnAllowed(true), "already CDespawnState must fail admission") && ok;

    return ok;
}
