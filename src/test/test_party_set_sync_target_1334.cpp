#include "test_party_set_sync_target_1334.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party set sync target 1334 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartySetSyncTarget1334SelfTests() -> bool
{
    using gate = partyhelpers::set_sync_target_gate;
    bool ok    = true;

    ok = expect(partyhelpers::LevelSyncMinLevel == 10, "min level") && ok;
    ok = expect(partyhelpers::LevelSyncDisableDurationSeconds == 30, "disable duration") && ok;
    ok = expect(partyhelpers::MsgLevelSyncDesigneeBelowMin == 541, "msg below min") && ok;
    ok = expect(partyhelpers::MsgLevelSyncDesigneeInOtherArea == 542, "msg other area") && ok;
    ok = expect(partyhelpers::MsgLevelSyncPreventedByStatus == 543, "msg status") && ok;

    ok = expect(partyhelpers::ClassifySetSyncTarget(false, true, true, 75, true, false) == gate::DISABLED, "disabled") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, false, false, 0, false, false) == gate::DISABLE, "no designee") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, false, 75, true, false) == gate::DISABLE, "non-pc designee") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 9, true, false) == gate::REJECT_BELOW_MIN, "below min") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 10, true, false) == gate::ENABLE, "level 10 ok") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 75, false, false) == gate::REJECT_OTHER_AREA, "other area") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 75, true, true) == gate::REJECT_STATUS, "blocking status") && ok;
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 75, true, false) == gate::ENABLE, "enable") && ok;

    // Precedence: below min before other area / status.
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 5, false, true) == gate::REJECT_BELOW_MIN, "below precedes zone/status") && ok;
    // Zone before status.
    ok = expect(partyhelpers::ClassifySetSyncTarget(true, true, true, 50, false, true) == gate::REJECT_OTHER_AREA, "zone precedes status") && ok;

    ok = expect(partyhelpers::ShouldApplySyncEnableToMember(true, true, true), "enable pc ok") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncEnableToMember(false, true, true), "enable non-pc") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncEnableToMember(true, false, true), "enable disappear") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncEnableToMember(true, true, false), "enable other zone") && ok;

    ok = expect(partyhelpers::ShouldApplySyncDisableToMember(true, true), "disable pc ok") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncDisableToMember(false, true), "disable non-pc") && ok;
    ok = expect(!partyhelpers::ShouldApplySyncDisableToMember(true, false), "disable disappear") && ok;

    ok = expect(partyhelpers::ShouldStartSyncDisableCountdown(true, true), "countdown infinite sync") && ok;
    ok = expect(!partyhelpers::ShouldStartSyncDisableCountdown(true, false), "no countdown timed sync") && ok;
    ok = expect(!partyhelpers::ShouldStartSyncDisableCountdown(false, true), "no countdown missing effect") && ok;

    return ok;
}
