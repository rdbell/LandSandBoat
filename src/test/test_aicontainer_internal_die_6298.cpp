#include "test_aicontainer_internal_die_6298.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Die 6298 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Die outer gate:
//   dynamic_cast<CBattleEntity*>(PEntity) != nullptr
auto inlineHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// Compact dual-wire pin matching Go pinInternalDieHasBattleEntity6298.
auto pinHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

} // namespace

// Pure dual-wire suite for Internal_Die outer battle-entity gate
// (OmegaXI internal/aicontainer; slice 6298).
//
// Coverage:
//   - free == inline == pin outer identity gate
//   - host-style inject poles (outer → ChangeState<CDeathState>+result vs false)
//   - residual independence (6291/6292/6294/6296 dual-wires / CanDispatch / CanChangeState)
auto runAicontainerInternalDie6298SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalChangeTargetHasBattleEntity;
    using aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget;
    using aicontainerhelpers::InternalDieHasBattleEntity;
    using aicontainerhelpers::InternalDisengageHasBattleEntity;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;

    bool ok = true;

    // --- Outer gate: free == inline == pin (identity) ---
    ok = expect(InternalDieHasBattleEntity(true), "has battle entity → ChangeState<CDeathState> + return result") && ok;
    ok = expect(!InternalDieHasBattleEntity(false), "no battle entity → return false") && ok;

    for (const bool hasBattle : { false, true })
    {
        const bool got     = InternalDieHasBattleEntity(hasBattle);
        const bool inlineF = inlineHasBattleEntity(hasBattle);
        const bool pinGot  = pinHasBattleEntity(hasBattle);
        ok                 = expect(got == hasBattle, "outer free==identity") && ok;
        ok                 = expect(got == inlineF, "outer free==inline") && ok;
        ok                 = expect(got == pinGot, "outer free==pin") && ok;
    }

    // Host-style inject poles: outer → ChangeState<CDeathState>+result vs return false.
    const struct
    {
        bool        hasBattleEntity;
        bool        wantProceed;
        const char* label;
    } hostCases[] = {
        { true, true, "battle entity → ChangeState<CDeathState> + return result" },
        { false, false, "no battle entity → return false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalDieHasBattleEntity(c.hasBattleEntity);
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

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6298") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6298") &&
         ok;

    // Host evaluates outer first.
    ok = expect(InternalDieHasBattleEntity(true), "battle entity must pass outer gate") && ok;
    ok = expect(!InternalDieHasBattleEntity(false), "non-battle entity must fail outer gate") && ok;

    return ok;
}
