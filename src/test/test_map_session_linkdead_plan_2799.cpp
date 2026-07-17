#include "test_map_session_linkdead_plan_2799.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map session linkdead plan 2799 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectPlanActions(
    const mapsessionhelpers::LinkDeadTransitionPlan& plan,
    const mapsessionhelpers::LinkDeadTransitionAction* want,
    const uint8                                        wantCount,
    const char* const                                  label) -> bool
{
    if (plan.count != wantCount)
    {
        return expect(false, label);
    }
    for (uint8 i = 0; i < wantCount; ++i)
    {
        if (plan.actions[i] != want[i])
        {
            return expect(false, label);
        }
    }
    return expect(true, label);
}

} // namespace

auto runMapSessionLinkDeadPlan2799SelfTests() -> bool
{
    using mapsessionhelpers::LinkDeadTransitionAction;
    using mapsessionhelpers::PendingTimeoutCleanupAction;
    using mapsessionhelpers::PlanLinkDeadMark;
    using mapsessionhelpers::PlanLinkDeadRecover;
    using mapsessionhelpers::PlanPendingTimeoutCleanup;
    using mapsessionhelpers::ShouldMarkLinkDead;
    using mapsessionhelpers::ShouldRecoverLinkDead;

    bool ok = true;

    // --- ShouldMarkLinkDead truth table (caller already ensured >5s inactive) ---
    ok = expect(ShouldMarkLinkDead(true, false), "mark: has char, not linkdead") && ok;
    ok = expect(!ShouldMarkLinkDead(true, true), "mark: already linkdead skips") && ok;
    ok = expect(!ShouldMarkLinkDead(false, false), "mark: no char rejects") && ok;
    ok = expect(!ShouldMarkLinkDead(false, true), "mark: no char ignores already flag") && ok;

    // --- ShouldRecoverLinkDead truth table (caller ensured not inactive) ---
    ok = expect(ShouldRecoverLinkDead(true, true), "recover: has char, is linkdead") && ok;
    ok = expect(!ShouldRecoverLinkDead(true, false), "recover: not linkdead skips") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, true), "recover: no char rejects") && ok;
    ok = expect(!ShouldRecoverLinkDead(false, false), "recover: no char not linkdead") && ok;

    // Mark and recover gates are mutually exclusive on the same isLinkDead state
    // for a live character (caller still splits on the 5s inactive branch).
    ok = expect(ShouldMarkLinkDead(true, false) && !ShouldRecoverLinkDead(true, false),
                "fresh char: mark not recover") &&
        ok;
    ok = expect(!ShouldMarkLinkDead(true, true) && ShouldRecoverLinkDead(true, true),
                "linkdead char: recover not mark") &&
        ok;

    // --- PlanLinkDeadMark order: status normal vs not ---
    {
        const LinkDeadTransitionAction wantNormal[] = {
            LinkDeadTransitionAction::SetDisconnectingFlag,
            LinkDeadTransitionAction::SetLinkDead,
            LinkDeadTransitionAction::SetUpdateHPMask,
            LinkDeadTransitionAction::SpawnPCsIfNormal,
        };
        ok = expectPlanActions(PlanLinkDeadMark(true), wantNormal, 4, "mark normal order") && ok;

        const LinkDeadTransitionAction wantAbnormal[] = {
            LinkDeadTransitionAction::SetDisconnectingFlag,
            LinkDeadTransitionAction::SetLinkDead,
            LinkDeadTransitionAction::SetUpdateHPMask,
        };
        ok = expectPlanActions(PlanLinkDeadMark(false), wantAbnormal, 3, "mark non-normal order") && ok;
    }

    // --- PlanLinkDeadRecover order: status normal vs not ---
    {
        const LinkDeadTransitionAction wantNormal[] = {
            LinkDeadTransitionAction::ClearDisconnectingFlag,
            LinkDeadTransitionAction::ClearLinkDead,
            LinkDeadTransitionAction::SetUpdateHPMask,
            LinkDeadTransitionAction::SpawnPCsIfNormal,
            LinkDeadTransitionAction::SaveCharStats,
        };
        ok = expectPlanActions(PlanLinkDeadRecover(true), wantNormal, 5, "recover normal order") && ok;

        const LinkDeadTransitionAction wantAbnormal[] = {
            LinkDeadTransitionAction::ClearDisconnectingFlag,
            LinkDeadTransitionAction::ClearLinkDead,
            LinkDeadTransitionAction::SetUpdateHPMask,
            LinkDeadTransitionAction::SaveCharStats,
        };
        ok = expectPlanActions(PlanLinkDeadRecover(false), wantAbnormal, 4, "recover non-normal order") && ok;
    }

    // Mark never includes Clear* or SaveCharStats; recover never includes SetDisconnecting/SetLinkDead.
    {
        const auto mark    = PlanLinkDeadMark(true);
        const auto recover = PlanLinkDeadRecover(true);
        bool       markHasClearOrSave = false;
        for (uint8 i = 0; i < mark.count; ++i)
        {
            if (mark.actions[i] == LinkDeadTransitionAction::ClearDisconnectingFlag ||
                mark.actions[i] == LinkDeadTransitionAction::ClearLinkDead ||
                mark.actions[i] == LinkDeadTransitionAction::SaveCharStats)
            {
                markHasClearOrSave = true;
            }
        }
        bool recoverHasSet = false;
        for (uint8 i = 0; i < recover.count; ++i)
        {
            if (recover.actions[i] == LinkDeadTransitionAction::SetDisconnectingFlag ||
                recover.actions[i] == LinkDeadTransitionAction::SetLinkDead)
            {
                recoverHasSet = true;
            }
        }
        ok = expect(!markHasClearOrSave, "mark excludes clear/save") && ok;
        ok = expect(!recoverHasSet, "recover excludes set disconnect/linkdead") && ok;

        // SaveCharStats is always last on recover.
        ok = expect(recover.count > 0 &&
                        recover.actions[recover.count - 1] == LinkDeadTransitionAction::SaveCharStats,
                    "recover ends with SaveCharStats") &&
            ok;
    }

    // --- PlanPendingTimeoutCleanup fixed order ---
    {
        const auto plan = PlanPendingTimeoutCleanup();
        ok              = expect(plan.count == 3, "pending plan count") && ok;
        ok              = expect(plan.actions[0] == PendingTimeoutCleanupAction::DeleteDatabaseSession,
                    "pending delete db") &&
            ok;
        ok = expect(plan.actions[1] == PendingTimeoutCleanupAction::RemovePendingIndex,
                    "pending remove index") &&
            ok;
        ok = expect(plan.actions[2] == PendingTimeoutCleanupAction::ErasePending,
                    "pending erase") &&
            ok;
    }

    return ok;
}
