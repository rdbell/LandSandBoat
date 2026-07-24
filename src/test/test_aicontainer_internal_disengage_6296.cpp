#include "test_aicontainer_internal_disengage_6296.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Disengage 6296 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Disengage outer gate:
//   dynamic_cast<CBattleEntity*>(PEntity) != nullptr
auto inlineHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// Compact dual-wire pin matching Go pinInternalDisengageHasBattleEntity6296.
auto pinHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

} // namespace

// Pure dual-wire suite for Internal_Disengage outer battle-entity gate
// (OmegaXI internal/aicontainer; slice 6296).
//
// Coverage:
//   - free == inline == pin outer identity gate
//   - host-style inject poles (outer → SetBattleTargetID(0)+true vs false)
//   - residual independence (6291/6292/6294 dual-wires / CanDispatch / CanChangeState)
auto runAicontainerInternalDisengage6296SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalChangeTargetHasBattleEntity;
    using aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget;
    using aicontainerhelpers::InternalDisengageHasBattleEntity;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;

    bool ok = true;

    // --- Outer gate: free == inline == pin (identity) ---
    ok = expect(InternalDisengageHasBattleEntity(true), "has battle entity → SetBattleTargetID(0) + true") && ok;
    ok = expect(!InternalDisengageHasBattleEntity(false), "no battle entity → return false") && ok;

    for (const bool hasBattle : { false, true })
    {
        const bool got     = InternalDisengageHasBattleEntity(hasBattle);
        const bool inlineF = inlineHasBattleEntity(hasBattle);
        const bool pinGot  = pinHasBattleEntity(hasBattle);
        ok                 = expect(got == hasBattle, "outer free==identity") && ok;
        ok                 = expect(got == inlineF, "outer free==inline") && ok;
        ok                 = expect(got == pinGot, "outer free==pin") && ok;
    }

    // Host-style inject poles: outer → SetBattleTargetID(0)+true vs return false.
    const struct
    {
        bool        hasBattleEntity;
        bool        wantProceed;
        const char* label;
    } hostCases[] = {
        { true, true, "battle entity → SetBattleTargetID(0) + true" },
        { false, false, "no battle entity → return false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalDisengageHasBattleEntity(c.hasBattleEntity);
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

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6296") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6296") &&
         ok;

    // Host evaluates outer first.
    ok = expect(InternalDisengageHasBattleEntity(true), "battle entity must pass outer gate") && ok;
    ok = expect(!InternalDisengageHasBattleEntity(false), "non-battle entity must fail outer gate") && ok;

    return ok;
}
