#include "test_aicontainer_internal_engage_6291.h"

#include "map/ai/ai_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "aicontainer Internal_Engage 6291 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CAIContainer::Internal_Engage not-yet-engaged ForceChangeState admission OR:
//   CanChangeState()
//     || (GetCurrentState() && GetCurrentState()->IsCompleted())
//     || HasPreventActionEffect(true)
auto inlineForceAttackAllowed(
    const bool canChangeState,
    const bool hasCurrentState,
    const bool currentIsCompleted,
    const bool hasPreventActionIgnoringCharm) -> bool
{
    return canChangeState || (hasCurrentState && currentIsCompleted) || hasPreventActionIgnoringCharm;
}

// Compact dual-wire pin matching Go pinInternalEngageForceAttackAllowed6291.
auto pinForceAttackAllowed(
    const bool canChangeState,
    const bool hasCurrentState,
    const bool currentIsCompleted,
    const bool hasPreventActionIgnoringCharm) -> bool
{
    return canChangeState || (hasCurrentState && currentIsCompleted) || hasPreventActionIgnoringCharm;
}

// Inline post-OnEngage inactive resume:
//   HasPreventActionEffect(true) → Inactive(0ms, false)
auto inlineShouldResumeInactive(const bool hasPreventActionIgnoringCharm) -> bool
{
    return hasPreventActionIgnoringCharm;
}

auto pinShouldResumeInactive(const bool hasPreventActionIgnoringCharm) -> bool
{
    return hasPreventActionIgnoringCharm;
}

} // namespace

// Pure dual-wire suite for Internal_Engage not-yet-engaged admission and
// post-engage inactive resume (OmegaXI internal/aicontainer; slice 6291).
//
// Coverage:
//   - free == inline == pin admission OR
//   - residual poles: canChange / completed / prevent-action
//   - dense 2^4 free == inline == pin for admission
//   - post-engage resume identity free == inline == pin
//   - host-style inject poles
//   - residual independence (CanDispatch / CanChangeState left alone)
auto runAicontainerInternalEngage6291SelfTests() -> bool
{
    using aicontainerhelpers::CanChangeState;
    using aicontainerhelpers::CanDispatch;
    using aicontainerhelpers::InternalEngageForceAttackAllowed;
    using aicontainerhelpers::InternalEngageShouldResumeInactive;

    bool ok = true;

    // --- Core poles: free == inline == pin admission OR ---
    const struct
    {
        bool        canChangeState;
        bool        hasCurrentState;
        bool        currentIsCompleted;
        bool        hasPreventActionIgnoringCharm;
        bool        want;
        const char* label;
    } admissionCases[] = {
        // canChange alone admits.
        { true, false, false, false, true, "canChange alone admits" },
        { true, true, false, false, true, "canChange admits even when active incomplete" },

        // completed current state admits when change blocked.
        { false, true, true, false, true, "completed current admits without canChange" },
        { false, false, true, false, false, "idle ignores completed inject (no current)" },

        // prevent-action ignoring charm admits (brief attack switch).
        { false, true, false, true, true, "prevent-action admits despite blocked incomplete" },
        { false, false, false, true, true, "prevent-action admits when idle" },

        // all poles false rejects.
        { false, false, false, false, false, "idle no prevent no canChange rejects" },
        { false, true, false, false, false, "active incomplete no prevent rejects" },

        // combined poles still admit.
        { true, true, true, true, true, "all true admits" },
        { false, true, true, true, true, "completed + prevent admits" },
    };

    for (const auto& c : admissionCases)
    {
        const bool got = InternalEngageForceAttackAllowed(
            c.canChangeState, c.hasCurrentState, c.currentIsCompleted, c.hasPreventActionIgnoringCharm);
        const bool inlineF = inlineForceAttackAllowed(
            c.canChangeState, c.hasCurrentState, c.currentIsCompleted, c.hasPreventActionIgnoringCharm);
        const bool pinGot = pinForceAttackAllowed(
            c.canChangeState, c.hasCurrentState, c.currentIsCompleted, c.hasPreventActionIgnoringCharm);
        const bool wantPin = c.canChangeState || (c.hasCurrentState && c.currentIsCompleted) ||
                             c.hasPreventActionIgnoringCharm;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "admission dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "admission dual-wire free==pin") && ok;
        ok = expect(got == wantPin, "admission dual-wire free==OR pin form") && ok;
    }

    // Dense compose: full 2^4 boolean space free == inline == pin.
    for (const bool canChange : { false, true })
    {
        for (const bool hasCurrent : { false, true })
        {
            for (const bool completed : { false, true })
            {
                for (const bool prevent : { false, true })
                {
                    const bool got     = InternalEngageForceAttackAllowed(canChange, hasCurrent, completed, prevent);
                    const bool inlineF = inlineForceAttackAllowed(canChange, hasCurrent, completed, prevent);
                    const bool pinGot  = pinForceAttackAllowed(canChange, hasCurrent, completed, prevent);
                    const bool want    = canChange || (hasCurrent && completed) || prevent;
                    ok                 = expect(got == inlineF, "compose admission free==inline") && ok;
                    ok                 = expect(got == pinGot, "compose admission free==pin") && ok;
                    ok                 = expect(got == want, "compose admission free==OR form") && ok;
                }
            }
        }
    }

    // Host-style inject poles for Internal_Engage not-yet-engaged path.
    const struct
    {
        bool        canChangeState;
        bool        hasCurrentState;
        bool        currentIsCompleted;
        bool        hasPreventActionIgnoringCharm;
        bool        wantAttemptForce;
        const char* label;
    } hostCases[] = {
        { true, false, false, false, true, "CanChangeState true → attempt ForceChangeState" },
        { false, true, true, false, true, "completed current → attempt ForceChangeState" },
        { false, true, false, true, true, "HasPreventActionEffect(true) → attempt ForceChangeState" },
        { false, true, false, false, false, "blocked incomplete no prevent → skip ForceChangeState" },
        { false, false, false, false, false, "idle no canChange no prevent → skip ForceChangeState" },
    };

    for (const auto& c : hostCases)
    {
        const bool got = InternalEngageForceAttackAllowed(
            c.canChangeState, c.hasCurrentState, c.currentIsCompleted, c.hasPreventActionIgnoringCharm);
        ok = expect(got == c.wantAttemptForce, c.label) && ok;
    }

    // --- Post-engage inactive resume identity ---
    ok = expect(InternalEngageShouldResumeInactive(true), "prevent present → resume Inactive") && ok;
    ok = expect(!InternalEngageShouldResumeInactive(false), "no prevent → stay attack state") && ok;
    ok = expect(InternalEngageShouldResumeInactive(true) == inlineShouldResumeInactive(true),
                "resume free==inline true") &&
         ok;
    ok = expect(InternalEngageShouldResumeInactive(false) == inlineShouldResumeInactive(false),
                "resume free==inline false") &&
         ok;
    ok = expect(InternalEngageShouldResumeInactive(true) == pinShouldResumeInactive(true),
                "resume free==pin true") &&
         ok;
    ok = expect(InternalEngageShouldResumeInactive(false) == pinShouldResumeInactive(false),
                "resume free==pin false") &&
         ok;

    // Dense resume poles.
    for (const bool prevent : { false, true })
    {
        const bool got = InternalEngageShouldResumeInactive(prevent);
        ok             = expect(got == prevent, "resume identity compose") && ok;
        ok             = expect(got == inlineShouldResumeInactive(prevent), "resume compose free==inline") && ok;
        ok             = expect(got == pinShouldResumeInactive(prevent), "resume compose free==pin") && ok;
    }

    // Residual independence: sibling dual-wires left alone.
    ok = expect(CanChangeState(false, false) && CanChangeState(true, true),
                "CanChangeState residual still holds under 6291") &&
         ok;
    ok = expect(!CanChangeState(true, false), "CanChangeState active block residual still holds") && ok;
    ok = expect(!CanDispatch(false) && CanDispatch(true),
                "CanDispatch residual still holds under 6291") &&
         ok;

    // Admission is distinct from CanChangeState alone: prevent-action can admit
    // even when CanChangeState would reject.
    ok = expect(!CanChangeState(true, false), "CanChangeState rejects blocked active") && ok;
    ok = expect(InternalEngageForceAttackAllowed(false, true, false, true),
                "admission accepts prevent-action when CanChangeState would reject") &&
         ok;

    return ok;
}
