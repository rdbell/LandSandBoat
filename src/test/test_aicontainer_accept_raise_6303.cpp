#include "test_aicontainer_accept_raise_6303.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Accept_Raise 6303 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Accept_Raise outer admission:
//   isCurrentDeathState
auto inlineAcceptRaiseShouldInvoke(const bool isCurrentDeathState) -> bool
{
    return isCurrentDeathState;
}

// Compact dual-wire pin matching Go pinAcceptRaiseShouldInvoke6303.
auto pinAcceptRaiseShouldInvoke(const bool isCurrentDeathState) -> bool
{
    return isCurrentDeathState;
}

} // namespace

// Pure dual-wire suite for Accept_Raise death-state admission
// (OmegaXI internal/aicontainer; slice 6303).
//
// Coverage:
//   - free == inline == pin identity isCurrentDeathState gate
//   - host-style inject poles (acceptRaise vs skip; always-false residual noted)
//   - residual independence (6291/6292/6294/6296/6298/6300/6302 dual-wires /
//     CanDispatch / CanChangeState)
auto runAicontainerAcceptRaise6303SelfTests() -> bool
{
    using aicontainerhelpers::AcceptRaiseShouldInvoke;
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

    // --- Outer admission: free == inline == pin (identity) ---
    ok = expect(AcceptRaiseShouldInvoke(true), "CDeathState current → host acceptRaise()") && ok;
    ok = expect(!AcceptRaiseShouldInvoke(false), "not CDeathState → host skips acceptRaise") && ok;

    for (const bool isDeath : { false, true })
    {
        const bool got     = AcceptRaiseShouldInvoke(isDeath);
        const bool inlineF = inlineAcceptRaiseShouldInvoke(isDeath);
        const bool pinGot  = pinAcceptRaiseShouldInvoke(isDeath);
        ok                 = expect(got == isDeath, "free==identity") && ok;
        ok                 = expect(got == inlineF, "free==inline") && ok;
        ok                 = expect(got == pinGot, "free==pin") && ok;
    }

    // Host-style inject poles.
    const struct
    {
        bool        isCurrentDeathState;
        bool        wantInvoke;
        const char* label;
    } hostCases[] = {
        { true, true, "CDeathState → host acceptRaise()" },
        { false, false, "not CDeathState → skip acceptRaise" },
    };

    for (const auto& c : hostCases)
    {
        const bool invoke = AcceptRaiseShouldInvoke(c.isCurrentDeathState);
        ok                = expect(invoke == c.wantInvoke, c.label) && ok;
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

    // Residual independence: 6296/6298/6300 dual-wires still hold.
    ok = expect(InternalDisengageHasBattleEntity(true) && !InternalDisengageHasBattleEntity(false),
                "6296 residual: disengage outer gate still holds") &&
         ok;
    ok = expect(InternalDieHasBattleEntity(true) && !InternalDieHasBattleEntity(false),
                "6298 residual: die outer gate still holds") &&
         ok;
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "6300 residual: despawn admission still holds") &&
         ok;

    // Residual independence: 6302 action target dual-wire still holds.
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6303") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6303") &&
         ok;

    // Host evaluates admission first.
    ok = expect(AcceptRaiseShouldInvoke(true), "CDeathState must pass admission") && ok;
    ok = expect(!AcceptRaiseShouldInvoke(false), "non-death state must fail admission") && ok;

    return ok;
}
