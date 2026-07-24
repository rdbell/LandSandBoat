#include "test_aicontainer_internal_synth_6304.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Synth 6304 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Synth outer admission:
//   hasCharEntity && !isCurrentSynthState
auto inlineSynthAllowed(const bool hasCharEntity, const bool isCurrentSynthState) -> bool
{
    return hasCharEntity && !isCurrentSynthState;
}

// Compact dual-wire pin matching Go pinInternalSynthAllowed6304.
auto pinSynthAllowed(const bool hasCharEntity, const bool isCurrentSynthState) -> bool
{
    return hasCharEntity && !isCurrentSynthState;
}

} // namespace

// Pure dual-wire suite for Internal_Synth char-entity + not-already-synth
// admission (OmegaXI internal/aicontainer; slice 6304).
//
// Coverage:
//   - free == inline == pin (hasCharEntity && !isCurrentSynthState)
//   - host-style inject poles (ForceChangeState vs false)
//   - residual independence (6291–6303 dual-wires / CanDispatch / CanChangeState)
auto runAicontainerInternalSynth6304SelfTests() -> bool
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
    using aicontainerhelpers::InternalSynthAllowed;

    bool ok = true;

    // --- Admission: free == inline == pin ---
    ok = expect(InternalSynthAllowed(true, false), "char + not synth → ForceChangeState + result") && ok;
    ok = expect(!InternalSynthAllowed(true, true), "char + already CSynthState → false") && ok;
    ok = expect(!InternalSynthAllowed(false, false), "no char → false") && ok;
    ok = expect(!InternalSynthAllowed(false, true), "no char + synth inject → false") && ok;

    for (const bool hasChar : { false, true })
    {
        for (const bool isSynth : { false, true })
        {
            const bool got     = InternalSynthAllowed(hasChar, isSynth);
            const bool inlineF = inlineSynthAllowed(hasChar, isSynth);
            const bool pinGot  = pinSynthAllowed(hasChar, isSynth);
            const bool want    = hasChar && !isSynth;
            ok                 = expect(got == want, "free==want") && ok;
            ok                 = expect(got == inlineF, "free==inline") && ok;
            ok                 = expect(got == pinGot, "free==pin") && ok;
        }
    }

    // Host-style inject poles.
    const struct
    {
        bool        hasCharEntity;
        bool        isCurrentSynthState;
        bool        wantProceed;
        const char* label;
    } hostCases[] = {
        { true, false, true, "char + not synth → ForceChangeState + result" },
        { true, true, false, "char + already CSynthState → false" },
        { false, false, false, "no char → false" },
        { false, true, false, "no char + synth inject → false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalSynthAllowed(c.hasCharEntity, c.isCurrentSynthState);
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

    // Residual independence: 6294/6296/6298/6300 dual-wires still hold.
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
    ok = expect(InternalDespawnAllowed(false) && !InternalDespawnAllowed(true),
                "6300 residual: despawn admission still holds") &&
         ok;

    // Residual independence: 6302/6303 dual-wires still hold.
    ok = expect(!InternalActionTargetAllowed(true, true) && InternalActionTargetAllowed(true, false),
                "6302 residual: action target gate still holds") &&
         ok;
    ok = expect(AcceptRaiseShouldInvoke(true) && !AcceptRaiseShouldInvoke(false),
                "6303 residual: accept raise admission still holds") &&
         ok;

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6304") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6304") &&
         ok;

    // Host evaluates admission first.
    ok = expect(InternalSynthAllowed(true, false), "char not-in-synth must pass admission") && ok;
    ok = expect(!InternalSynthAllowed(true, true), "already CSynthState must fail admission") && ok;

    return ok;
}
