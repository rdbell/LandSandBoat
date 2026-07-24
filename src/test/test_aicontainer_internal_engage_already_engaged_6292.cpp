#include "test_aicontainer_internal_engage_already_engaged_6292.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Engage already-engaged 6292 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Engage already-engaged path selection:
//   entity && entity->PAI->IsEngaged()
auto inlineIsAlreadyEngagedPath(const bool hasBattleEntity, const bool isEngaged) -> bool
{
    return hasBattleEntity && isEngaged;
}

// Compact dual-wire pin matching Go pinInternalEngageIsAlreadyEngagedPath6292.
auto pinIsAlreadyEngagedPath(const bool hasBattleEntity, const bool isEngaged) -> bool
{
    return hasBattleEntity && isEngaged;
}

// Inline already-engaged retarget:
//   GetBattleTargetID() != targetid
auto inlineShouldRetarget(const uint16 currentBattleTargetID, const uint16 requestedTargetID) -> bool
{
    return currentBattleTargetID != requestedTargetID;
}

auto pinShouldRetarget(const uint16 currentBattleTargetID, const uint16 requestedTargetID) -> bool
{
    return currentBattleTargetID != requestedTargetID;
}

} // namespace

// Pure dual-wire suite for Internal_Engage already-engaged path selection and
// retarget decision (OmegaXI internal/aicontainer; slice 6292).
//
// Coverage:
//   - free == inline == pin path selection AND
//   - free == inline == pin retarget !=
//   - dense 2^2 free == inline == pin for path selection
//   - host-style inject poles (path + retarget → return)
//   - residual independence (6291 admission / CanDispatch / CanChangeState)
auto runAicontainerInternalEngageAlreadyEngaged6292SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageIsAlreadyEngagedPath;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;
    using aicontainerhelpers::InternalEngageShouldRetarget;

    bool ok = true;

    // --- Path selection: free == inline == pin ---
    const struct
    {
        bool        hasBattleEntity;
        bool        isEngaged;
        bool        want;
        const char* label;
    } pathCases[] = {
        { true, true, true, "battle entity engaged → already-engaged path" },
        { true, false, false, "battle entity not engaged → not already-engaged path" },
        { false, true, false, "no battle entity (isEngaged inject true still rejects)" },
        { false, false, false, "no battle entity not engaged → not already-engaged path" },
    };

    for (const auto& c : pathCases)
    {
        const bool got     = InternalEngageIsAlreadyEngagedPath(c.hasBattleEntity, c.isEngaged);
        const bool inlineF = inlineIsAlreadyEngagedPath(c.hasBattleEntity, c.isEngaged);
        const bool pinGot  = pinIsAlreadyEngagedPath(c.hasBattleEntity, c.isEngaged);
        const bool wantPin = c.hasBattleEntity && c.isEngaged;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "path dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "path dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "path dual-wire free==AND pin form") && ok;
    }

    // Dense compose: full 2^2 free == inline == pin for path selection.
    for (const bool hasBattle : { false, true })
    {
        for (const bool engaged : { false, true })
        {
            const bool got     = InternalEngageIsAlreadyEngagedPath(hasBattle, engaged);
            const bool inlineF = inlineIsAlreadyEngagedPath(hasBattle, engaged);
            const bool pinGot  = pinIsAlreadyEngagedPath(hasBattle, engaged);
            const bool want    = hasBattle && engaged;
            ok                 = expect(got == inlineF, "compose path free==inline") && ok;
            ok                 = expect(got == pinGot, "compose path free==pin") && ok;
            ok                 = expect(got == want, "compose path free==AND form") && ok;
        }
    }

    // --- Retarget: free == inline == pin ---
    const struct
    {
        uint16      current;
        uint16      requested;
        bool        want;
        const char* label;
    } retargetCases[] = {
        { 1, 2, true, "different targets → retarget" },
        { 2, 1, true, "swap targets → retarget" },
        { 1, 1, false, "same target → no ChangeTarget" },
        { 0, 0, false, "both zero → no ChangeTarget" },
        { 0, 1, true, "zero current to non-zero → retarget" },
        { 5, 0, true, "non-zero to zero → retarget" },
        { 0xFFFF, 0xFFFE, true, "high ids different → retarget" },
        { 0xFFFF, 0xFFFF, false, "high ids same → no ChangeTarget" },
    };

    for (const auto& c : retargetCases)
    {
        const bool got     = InternalEngageShouldRetarget(c.current, c.requested);
        const bool inlineF = inlineShouldRetarget(c.current, c.requested);
        const bool pinGot  = pinShouldRetarget(c.current, c.requested);
        const bool wantPin = c.current != c.requested;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "retarget dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "retarget dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "retarget dual-wire free==!= pin form") && ok;
    }

    // Host-style inject poles: path + retarget → return value on path.
    const struct
    {
        bool        hasBattleEntity;
        bool        isEngaged;
        uint16      currentTarget;
        uint16      requestedTarget;
        bool        wantOnPath;
        bool        wantRetarget;
        bool        wantReturn;
        const char* label;
    } hostCases[] = {
        { true, true, 1, 2, true, true, true, "engaged different target → ChangeTarget + true" },
        { true, true, 3, 3, true, false, false, "engaged same target → false no ChangeTarget" },
        { true, true, 0, 7, true, true, true, "engaged zero→nonzero → ChangeTarget + true" },
        { true, false, 1, 2, false, false, false, "not engaged → leave path (not-yet-engaged)" },
        { false, false, 1, 2, false, false, false, "no battle entity → leave path" },
        { false, true, 1, 2, false, false, false, "no entity even if engaged inject true → leave path" },
    };

    for (const auto& c : hostCases)
    {
        const bool onPath = InternalEngageIsAlreadyEngagedPath(c.hasBattleEntity, c.isEngaged);
        ok                = expect(onPath == c.wantOnPath, c.label) && ok;
        if (!onPath)
        {
            continue;
        }
        const bool retarget = InternalEngageShouldRetarget(c.currentTarget, c.requestedTarget);
        ok                  = expect(retarget == c.wantRetarget, c.label) && ok;
        // On already-engaged path, return value equals retarget decision.
        ok = expect(retarget == c.wantReturn, c.label) && ok;
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

    // Sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && !CanChangeState(true, false),
                "CanChangeState residual still holds under 6292") &&
         ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6292") &&
         ok;

    // Already-engaged path is independent of admission OR: host selects path first.
    ok = expect(InternalEngageIsAlreadyEngagedPath(true, true),
                "engaged battle entity must take already-engaged path") &&
         ok;
    ok = expect(!InternalEngageShouldRetarget(9, 9), "same target must not retarget") && ok;

    return ok;
}
