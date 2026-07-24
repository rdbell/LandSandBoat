#include "test_aicontainer_internal_change_target_6294.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_ChangeTarget 6294 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_ChangeTarget outer gate:
//   dynamic_cast<CBattleEntity*>(PEntity) != nullptr
auto inlineHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// Compact dual-wire pin matching Go pinInternalChangeTargetHasBattleEntity6294.
auto pinHasBattleEntity(const bool hasBattleEntity) -> bool
{
    return hasBattleEntity;
}

// Inline path split:
//   IsEngaged() || targetid == 0
auto inlineShouldSetBattleTarget(const bool isEngaged, const uint16 targetid) -> bool
{
    return isEngaged || targetid == 0;
}

auto pinShouldSetBattleTarget(const bool isEngaged, const uint16 targetid) -> bool
{
    return isEngaged || targetid == 0;
}

} // namespace

// Pure dual-wire suite for Internal_ChangeTarget outer battle-entity gate and
// IsEngaged||targetid==0 path split (OmegaXI internal/aicontainer; slice 6294).
//
// Coverage:
//   - free == inline == pin outer identity gate
//   - free == inline == pin path split OR
//   - dense 2^2 free == inline == pin for path (engaged x zero/nonzero)
//   - host-style inject poles (outer + path → SetBattleTargetID vs Engage)
//   - residual independence (6291/6292 engage dual-wires / CanDispatch / CanChangeState)
auto runAicontainerInternalChangeTarget6294SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalChangeTargetHasBattleEntity;
    using aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;

    bool ok = true;

    // --- Outer gate: free == inline == pin (identity) ---
    ok = expect(InternalChangeTargetHasBattleEntity(true), "has battle entity → proceed") && ok;
    ok = expect(!InternalChangeTargetHasBattleEntity(false), "no battle entity → return false") && ok;

    for (const bool hasBattle : { false, true })
    {
        const bool got     = InternalChangeTargetHasBattleEntity(hasBattle);
        const bool inlineF = inlineHasBattleEntity(hasBattle);
        const bool pinGot  = pinHasBattleEntity(hasBattle);
        ok                 = expect(got == hasBattle, "outer free==identity") && ok;
        ok                 = expect(got == inlineF, "outer free==inline") && ok;
        ok                 = expect(got == pinGot, "outer free==pin") && ok;
    }

    // --- Path split: free == inline == pin ---
    const struct
    {
        bool        isEngaged;
        uint16      targetid;
        bool        want;
        const char* label;
    } pathCases[] = {
        { true, 1, true, "engaged nonzero → SetBattleTargetID" },
        { true, 0, true, "engaged zero → SetBattleTargetID" },
        { false, 0, true, "not engaged zero → SetBattleTargetID (clear)" },
        { false, 1, false, "not engaged nonzero → Engage" },
        { false, 42, false, "not engaged other id → Engage" },
        { true, 0xFFFF, true, "engaged high id → SetBattleTargetID" },
        { false, 0xFFFF, false, "not engaged high id → Engage" },
    };

    for (const auto& c : pathCases)
    {
        const bool got     = InternalChangeTargetShouldSetBattleTarget(c.isEngaged, c.targetid);
        const bool inlineF = inlineShouldSetBattleTarget(c.isEngaged, c.targetid);
        const bool pinGot  = pinShouldSetBattleTarget(c.isEngaged, c.targetid);
        const bool wantPin = c.isEngaged || c.targetid == 0;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "path dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "path dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "path dual-wire free==OR pin form") && ok;
    }

    // Dense compose: engaged x {0, nonzero} free == inline == pin == OR form.
    for (const bool engaged : { false, true })
    {
        for (const uint16 tid : { static_cast<uint16>(0), static_cast<uint16>(1), static_cast<uint16>(0xFFFF) })
        {
            const bool got     = InternalChangeTargetShouldSetBattleTarget(engaged, tid);
            const bool inlineF = inlineShouldSetBattleTarget(engaged, tid);
            const bool pinGot  = pinShouldSetBattleTarget(engaged, tid);
            const bool want    = engaged || tid == 0;
            ok                 = expect(got == inlineF, "compose path free==inline") && ok;
            ok                 = expect(got == pinGot, "compose path free==pin") && ok;
            ok                 = expect(got == want, "compose path free==OR form") && ok;
        }
    }

    // Host-style inject poles: outer + path → SetBattleTargetID vs Engage vs false.
    const struct
    {
        bool        hasBattleEntity;
        bool        isEngaged;
        uint16      targetid;
        bool        wantProceed;
        bool        wantSetBattleTarget; // meaningful only when proceed
        // When proceed && set → return true after SetBattleTargetID
        // When proceed && !set → return Engage(targetid) (host residual)
        // When !proceed → return false
        const char* label;
    } hostCases[] = {
        { true, true, 2, true, true, "battle engaged → SetBattleTargetID + true" },
        { true, true, 0, true, true, "battle engaged zero → SetBattleTargetID + true" },
        { true, false, 0, true, true, "battle not engaged zero → SetBattleTargetID + true" },
        { true, false, 5, true, false, "battle not engaged nonzero → Engage" },
        { false, false, 5, false, false, "no battle entity → return false" },
        { false, true, 0, false, false, "no battle entity even if engaged inject → false" },
    };

    for (const auto& c : hostCases)
    {
        const bool proceed = InternalChangeTargetHasBattleEntity(c.hasBattleEntity);
        ok                 = expect(proceed == c.wantProceed, c.label) && ok;
        if (!proceed)
        {
            continue;
        }
        const bool setTarget = InternalChangeTargetShouldSetBattleTarget(c.isEngaged, c.targetid);
        ok                   = expect(setTarget == c.wantSetBattleTarget, c.label) && ok;
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

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6294") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6294") &&
         ok;

    // Path is independent of outer gate: host evaluates outer first.
    ok = expect(InternalChangeTargetHasBattleEntity(true), "battle entity must pass outer gate") && ok;
    ok = expect(InternalChangeTargetShouldSetBattleTarget(false, 0), "zero target must set battle target") && ok;
    ok = expect(!InternalChangeTargetShouldSetBattleTarget(false, 9), "not engaged nonzero must Engage") && ok;

    return ok;
}
