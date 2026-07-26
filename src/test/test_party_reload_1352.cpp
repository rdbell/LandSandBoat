#include "test_party_reload_1352.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party reload 1352 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyReload1352SelfTests() -> bool
{
    using reload_path = partyhelpers::reload_party_path;
    using treasure_scan = partyhelpers::reload_treasure_scan;
    bool ok             = true;

    // ReloadParty
    ok = expect(partyhelpers::ShouldSkipMobReloadParty(true), "skip mob") && ok;
    ok = expect(!partyhelpers::ShouldSkipMobReloadParty(false), "pc reload") && ok;
    ok = expect(partyhelpers::ClassifyReloadPartyPath(true) == reload_path::ALLIANCE, "alliance path") && ok;
    ok = expect(partyhelpers::ClassifyReloadPartyPath(false) == reload_path::PARTY, "party path") && ok;

    // ReloadPartyMembers null
    ok = expect(partyhelpers::ShouldRejectNullReloadPartyMembers(true), "members null") && ok;
    ok = expect(!partyhelpers::ShouldRejectNullReloadPartyMembers(false), "members ok") && ok;
    ok = expect(partyhelpers::FormatReloadPartyMembersNullWarning() ==
                    "CParty::ReloadPartyMembers() - PChar was null.",
                "members warn") &&
         ok;

    // Alliance list index
    ok = expect(partyhelpers::AlliancePartySlotMask == 0x0003, "slot mask") && ok;
    ok = expect(partyhelpers::ShouldResetAllianceListIndex(0x0001, 0), "reset to second") && ok;
    ok = expect(!partyhelpers::ShouldResetAllianceListIndex(0x0001, 0x0001), "same second") && ok;
    ok = expect(partyhelpers::ShouldResetAllianceListIndex(0x0002, 0x0001), "second to third") && ok;
    ok = expect(partyhelpers::NextAllianceListCursor(0x0005) == 0x0001, "cursor second") && ok;
    ok = expect(partyhelpers::NextAllianceListCursor(0x0006) == 0x0002, "cursor third") && ok;

    partyhelpers::reload_party_member_list_position listPosition{};
    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, 0x0000);
    ok = expect(listPosition.allianceCursor == 0 && listPosition.listIndex == 0, "main first row") && ok;
    listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, 0x0000);
    ok = expect(listPosition.allianceCursor == 0 && listPosition.listIndex == 1, "main next row") && ok;
    listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, 0x0001);
    ok = expect(listPosition.allianceCursor == 0x0001 && listPosition.listIndex == 0, "second resets row") && ok;
    listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, 0x0002);
    ok = expect(listPosition.allianceCursor == 0x0002 && listPosition.listIndex == 0, "third resets row") && ok;
    listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, 0x0000);
    ok = expect(listPosition.allianceCursor == 0 && listPosition.listIndex == 0, "main resets row") && ok;

    const uint16 allianceFlags[] = { 0x0004, 0x0010, 0x0001, 0x0005, 0x0002, 0x0006 };
    const uint8  allianceIndexes[] = { 0, 1, 0, 1, 0, 1 };
    listPosition = {};
    for (std::size_t i = 0; i < sizeof(allianceFlags) / sizeof(allianceFlags[0]); ++i)
    {
        listPosition = partyhelpers::BeginReloadPartyMemberListRow(listPosition, allianceFlags[i]);
        ok = expect(listPosition.listIndex == allianceIndexes[i], "alliance row sequence") && ok;
        listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    }

    for (uint16 i = 0; i < 256; ++i)
    {
        listPosition = partyhelpers::AdvanceReloadPartyMemberListRow(listPosition);
    }
    ok = expect(listPosition.listIndex == 0, "index wraps") && ok;

    auto soloRow = partyhelpers::PlanSoloReloadPartyRow(4, true, 2);
    ok = expect(soloRow.memberIndex == 4 && soloRow.nextIndex == 7, "online row trusts") && ok;
    soloRow = partyhelpers::PlanSoloReloadPartyRow(4, false, 2);
    ok = expect(soloRow.memberIndex == 4 && soloRow.nextIndex == 5, "offline row no trusts") && ok;
    soloRow = partyhelpers::PlanSoloReloadPartyRow(255, true, 1);
    ok = expect(soloRow.memberIndex == 255 && soloRow.nextIndex == 1, "trust row wraps") && ok;

    // Offline zone
    ok = expect(partyhelpers::OfflineMemberZoneID(0, 100) == 100, "prev zone") && ok;
    ok = expect(partyhelpers::OfflineMemberZoneID(50, 100) == 50, "current zone") && ok;

    // ReloadTreasurePool
    ok = expect(partyhelpers::ShouldRejectNullReloadTreasurePool(true), "pool null") && ok;
    ok = expect(!partyhelpers::ShouldRejectNullReloadTreasurePool(false), "pool ok") && ok;
    ok = expect(partyhelpers::FormatReloadTreasurePoolNullWarning() ==
                    "CParty::ReloadTreasurePool() - PChar was null.",
                "pool warn") &&
         ok;
    ok = expect(partyhelpers::ShouldKeepZoneTreasurePool(true, true), "keep zone") && ok;
    ok = expect(!partyhelpers::ShouldKeepZoneTreasurePool(true, false), "not zone") && ok;
    ok = expect(!partyhelpers::ShouldKeepZoneTreasurePool(false, true), "no pool") && ok;

    ok = expect(partyhelpers::ClassifyReloadTreasureScan(false, false) == treasure_scan::NONE, "scan none") && ok;
    ok = expect(partyhelpers::ClassifyReloadTreasureScan(true, true) == treasure_scan::ALLIANCE, "scan alliance") && ok;
    ok = expect(partyhelpers::ClassifyReloadTreasureScan(true, false) == treasure_scan::PARTY, "scan party") && ok;

    ok = expect(partyhelpers::ShouldJoinMemberTreasurePool(false, true, true), "join") && ok;
    ok = expect(!partyhelpers::ShouldJoinMemberTreasurePool(true, true, true), "self") && ok;
    ok = expect(!partyhelpers::ShouldJoinMemberTreasurePool(false, false, true), "no pool") && ok;
    ok = expect(!partyhelpers::ShouldJoinMemberTreasurePool(false, true, false), "diff zone") && ok;

    ok = expect(partyhelpers::ShouldDelOwnPoolBeforeJoin(true), "del own") && ok;
    ok = expect(!partyhelpers::ShouldDelOwnPoolBeforeJoin(false), "no own") && ok;
    ok = expect(partyhelpers::ShouldCreateSoloTreasurePool(false), "create solo") && ok;
    ok = expect(!partyhelpers::ShouldCreateSoloTreasurePool(true), "has pool") && ok;

    // RefreshFlags party filter + alliance leader
    ok = expect(partyhelpers::ShouldRefreshFlagsForParty(10, 10), "same party") && ok;
    ok = expect(!partyhelpers::ShouldRefreshFlagsForParty(10, 11), "other party") && ok;
    ok = expect(partyhelpers::ShouldAssignAllianceLeaderFromFlags(0x0008, true), "ally leader") && ok;
    ok = expect(!partyhelpers::ShouldAssignAllianceLeaderFromFlags(0x0008, false), "no alliance") && ok;
    ok = expect(!partyhelpers::ShouldAssignAllianceLeaderFromFlags(0x0004, true), "not ally flag") && ok;

    return ok;
}
