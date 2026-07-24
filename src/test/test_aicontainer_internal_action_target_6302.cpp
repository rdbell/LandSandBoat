#include "test_aicontainer_internal_action_target_6302.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_* action target 6302 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Internal_* untargetable target gate:
//   !hasTarget || !isUntargetable
auto inlineActionTargetAllowed(const bool hasTarget, const bool isUntargetable) -> bool
{
    return !hasTarget || !isUntargetable;
}

// Compact dual-wire pin matching Go pinInternalActionTargetAllowed6302.
auto pinActionTargetAllowed(const bool hasTarget, const bool isUntargetable) -> bool
{
    return !hasTarget || !isUntargetable;
}

} // namespace

// Pure dual-wire suite for Internal_Cast/WeaponSkill/MobSkill/PetSkill/
// Ability/RangedAttack target untargetable gate
// (OmegaXI internal/aicontainer; slice 6302).
//
// Coverage:
//   - free == inline == pin (!hasTarget || !isUntargetable)
//   - host-style inject poles (ChangeState vs return false)
//   - residual independence (6291/6292/6294/6296/6298/6300 dual-wires /
//     CanDispatch / CanChangeState)
auto runAicontainerInternalActionTarget6302SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
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

    bool ok = true;

    // --- Gate: free == inline == pin (!hasTarget || !isUntargetable) ---
    ok = expect(InternalActionTargetAllowed(false, false), "no target → ChangeState + return result") && ok;
    ok = expect(InternalActionTargetAllowed(false, true), "no target with untargetable inject still allows") && ok;
    ok = expect(InternalActionTargetAllowed(true, false), "targetable target → ChangeState + return result") && ok;
    ok = expect(!InternalActionTargetAllowed(true, true), "untargetable target → return false") && ok;

    for (const bool hasTarget : { false, true })
    {
        for (const bool isUntargetable : { false, true })
        {
            const bool got     = InternalActionTargetAllowed(hasTarget, isUntargetable);
            const bool inlineF = inlineActionTargetAllowed(hasTarget, isUntargetable);
            const bool pinGot  = pinActionTargetAllowed(hasTarget, isUntargetable);
            const bool want    = !hasTarget || !isUntargetable;
            ok                 = expect(got == want, "free==want") && ok;
            ok                 = expect(got == inlineF, "free==inline") && ok;
            ok                 = expect(got == pinGot, "free==pin") && ok;
        }
    }

    // Host-style inject poles.
    const struct
    {
        bool        hasTarget;
        bool        isUntargetable;
        bool        wantProceed;
        const char* label;
    } hostCases[] = {
        { false, false, true, "no target → ChangeState + return result" },
        { false, true, true, "no target (untargetable inject ignored) → ChangeState" },
        { true, false, true, "targetable target → ChangeState + return result" },
        { true, true, false, "untargetable target → return false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalActionTargetAllowed(c.hasTarget, c.isUntargetable);
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

    // Residual independence: 6300 Despawn dual-wire still holds.
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "6300 residual: despawn admission still holds") &&
         ok;

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6302") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6302") &&
         ok;

    // Host evaluates gate after outer entity check.
    ok = expect(!InternalActionTargetAllowed(true, true), "untargetable target must fail gate") && ok;
    ok = expect(InternalActionTargetAllowed(true, false), "targetable target must pass gate") && ok;

    return ok;
}
