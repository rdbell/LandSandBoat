#include "test_status_save_plan_2793.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status save plan 2793 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectAction(
    const statuseffecthelpers::SaveStatusEffectPlan& plan,
    const statuseffecthelpers::SaveEffectAction      wantAction,
    const bool                                       wantUtsu,
    const bool                                       wantBlink,
    const bool                                       wantSkin,
    const char* const                                label) -> bool
{
    const bool ok = plan.action == wantAction && plan.resyncUtsusemi == wantUtsu && plan.resyncBlink == wantBlink &&
                    plan.resyncStoneskin == wantSkin;
    return expect(ok, label);
}

} // namespace

auto runStatusSavePlan2793SelfTests() -> bool
{
    using statuseffecthelpers::PlanSaveStatusEffect;
    using statuseffecthelpers::SaveEffectAction;
    using statuseffecthelpers::ShouldPersistEffect;
    using statuseffecthelpers::ShouldResyncBlinkPower;
    using statuseffecthelpers::ShouldResyncStoneskinPower;
    using statuseffecthelpers::ShouldResyncUtsusemiPower;
    using statuseffecthelpers::ShouldSkipDeletedOnSave;
    using statuseffecthelpers::ShouldStripOnSave;
    using statuseffecthelpers::StatusIDBlink;
    using statuseffecthelpers::StatusIDCopyImage;
    using statuseffecthelpers::StatusIDStoneskin;

    bool ok = true;

    // 1) strip runs even when deleted (LSB strip-first order).
    ok = expectAction(PlanSaveStatusEffect(true, true, true, false, 10, 30, StatusIDBlink), SaveEffectAction::Strip, false, false, false, "deleted strip wins") &&
        ok;
    // deleted without strip flags → SkipDeleted.
    ok = expectAction(PlanSaveStatusEffect(true, false, false, false, 0, 0, 1), SaveEffectAction::SkipDeleted, false, false, false, "deleted permanent") &&
        ok;

    // 2) strip on logout (Logout flag) / zone (OnZone flag).
    ok = expectAction(PlanSaveStatusEffect(false, true, true, false, 10, 30, 1), SaveEffectAction::Strip, false, false, false, "logout strip") &&
        ok;
    ok = expectAction(PlanSaveStatusEffect(false, false, false, true, 10, 30, 1), SaveEffectAction::Strip, false, false, false, "zone strip") &&
        ok;
    // strip wins over persist inputs (remaining > 0).
    ok = expectAction(PlanSaveStatusEffect(false, true, true, true, 99, 99, StatusIDCopyImage), SaveEffectAction::Strip, false, false, false, "strip no resync") &&
        ok;

    // 3) persist ordinary effect → Persist, no resync.
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 10, 30, 1), SaveEffectAction::Persist, false, false, false, "persist plain") &&
        ok;
    // permanent (duration 0) persists even with realDuration 0.
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 0, 0, 1), SaveEffectAction::Persist, false, false, false, "persist permanent") &&
        ok;

    // resync flags only on Persist, exclusive if/else-if chain.
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 10, 30, StatusIDCopyImage), SaveEffectAction::Persist, true, false, false, "resync utsusemi") &&
        ok;
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 10, 30, StatusIDBlink), SaveEffectAction::Persist, false, true, false, "resync blink") &&
        ok;
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 10, 30, StatusIDStoneskin), SaveEffectAction::Persist, false, false, true, "resync stoneskin") &&
        ok;

    // 4) expired non-permanent → DropNoPersist.
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 0, 30, 1), SaveEffectAction::DropNoPersist, false, false, false, "drop expired") &&
        ok;
    ok = expectAction(PlanSaveStatusEffect(false, false, false, false, 0, 30, StatusIDBlink), SaveEffectAction::DropNoPersist, false, false, false, "drop no resync") &&
        ok;

    // No strip when flags do not match mode (logout without Logout; zone without OnZone).
    ok = expectAction(PlanSaveStatusEffect(false, true, false, true, 10, 30, 1), SaveEffectAction::Persist, false, false, false, "logout no Logout flag") &&
        ok;
    ok = expectAction(PlanSaveStatusEffect(false, false, true, false, 10, 30, 1), SaveEffectAction::Persist, false, false, false, "zone no OnZone flag") &&
        ok;

    // Compose existing helpers: plan action must match Should* short-circuit order.
    ok = expect(ShouldSkipDeletedOnSave(true) && !ShouldSkipDeletedOnSave(false), "helper deleted") && ok;
    ok = expect(ShouldStripOnSave(true, true, false) && ShouldStripOnSave(false, false, true), "helper strip") && ok;
    ok = expect(!ShouldStripOnSave(true, false, true) && !ShouldStripOnSave(false, true, false), "helper no strip") && ok;
    ok = expect(ShouldPersistEffect(10, 30) && ShouldPersistEffect(0, 0) && !ShouldPersistEffect(0, 30), "helper persist") && ok;
    ok = expect(ShouldResyncUtsusemiPower(StatusIDCopyImage) && ShouldResyncBlinkPower(StatusIDBlink), "helper resync") && ok;
    ok = expect(ShouldResyncStoneskinPower(StatusIDStoneskin), "helper resync skin") && ok;

    const struct
    {
        bool        deleted;
        bool        logout;
        bool        hasLogoutFlag;
        bool        hasOnZoneFlag;
        int64       realDuration;
        int64       duration;
        uint16      statusID;
        const char* label;
    } composeCases[] = {
        { true, false, false, false, 10, 30, 1, "compose deleted" },
        { false, true, true, false, 10, 30, StatusIDBlink, "compose strip" },
        { false, false, false, false, 10, 30, StatusIDCopyImage, "compose persist utsu" },
        { false, false, false, false, 0, 30, StatusIDStoneskin, "compose drop" },
        { false, false, false, false, 0, 0, StatusIDBlink, "compose permanent blink" },
    };
    for (const auto& c : composeCases)
    {
        const auto plan = PlanSaveStatusEffect(c.deleted, c.logout, c.hasLogoutFlag, c.hasOnZoneFlag, c.realDuration, c.duration, c.statusID);

        SaveEffectAction want = SaveEffectAction::DropNoPersist;
        bool             u = false, b = false, s = false;
        if (ShouldStripOnSave(c.logout, c.hasLogoutFlag, c.hasOnZoneFlag))
        {
            want = SaveEffectAction::Strip;
        }
        else if (ShouldSkipDeletedOnSave(c.deleted))
        {
            want = SaveEffectAction::SkipDeleted;
        }
        else if (ShouldPersistEffect(c.realDuration, c.duration))
        {
            want = SaveEffectAction::Persist;
            if (ShouldResyncUtsusemiPower(c.statusID))
            {
                u = true;
            }
            else if (ShouldResyncBlinkPower(c.statusID))
            {
                b = true;
            }
            else if (ShouldResyncStoneskinPower(c.statusID))
            {
                s = true;
            }
        }
        ok = expectAction(plan, want, u, b, s, c.label) && ok;
    }

    return ok;
}
