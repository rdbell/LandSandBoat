#include "test_party_push_pop_del_1351.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party push/pop/del 1351 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyPushPopDel1351SelfTests() -> bool
{
    using entity_gate = partyhelpers::entity_party_gate;
    using del_path    = partyhelpers::del_member_path;
    using push_gate   = partyhelpers::push_member_gate;
    bool ok           = true;

    // Out-of-zone AddMember
    ok = expect(partyhelpers::ShouldRunOutOfZoneAddMember(true), "outzone pc") && ok;
    ok = expect(!partyhelpers::ShouldRunOutOfZoneAddMember(false), "outzone mob skip") && ok;
    ok = expect(partyhelpers::FormatAddMemberOutOfZoneFullWarning() ==
                    "CParty::AddMember() - Party was full when trying to add a member from out of zone.",
                "outzone full warn") &&
         ok;
    ok = expect(partyhelpers::OutOfZoneAddMemberFlags(false, 1) == 0, "no alliance flags") && ok;
    ok = expect(partyhelpers::OutOfZoneAddMemberFlags(true, 0) == 0, "main party flags") && ok;
    ok = expect(partyhelpers::OutOfZoneAddMemberFlags(true, 1) == partyhelpers::PartySecondFlag, "second") && ok;
    ok = expect(partyhelpers::OutOfZoneAddMemberFlags(true, 2) == partyhelpers::PartyThirdFlag, "third") && ok;
    ok = expect(partyhelpers::PartySecondFlag == 0x0001, "second const") && ok;
    ok = expect(partyhelpers::PartyThirdFlag == 0x0002, "third const") && ok;

    // DelMember / PopMember admission
    ok = expect(partyhelpers::ClassifyEntityPartyMatch(true, false) == entity_gate::REJECT_NULL_OR_MISMATCH, "null") && ok;
    ok = expect(partyhelpers::ClassifyEntityPartyMatch(false, true) == entity_gate::REJECT_NULL_OR_MISMATCH, "mismatch") && ok;
    ok = expect(partyhelpers::ClassifyEntityPartyMatch(false, false) == entity_gate::PROCEED, "match ok") && ok;
    ok = expect(partyhelpers::FormatDelMemberNullWarning() ==
                    "CParty::DelMember() - PEntity was null, or PParty mismatch.",
                "del warn") &&
         ok;
    ok = expect(partyhelpers::FormatPopMemberNullWarning() ==
                    "CParty::PopMember() - PEntity was null, or PParty mismatch.",
                "pop warn") &&
         ok;

    ok = expect(partyhelpers::ClassifyDelMemberPath(true) == del_path::AS_LEADER, "del leader") && ok;
    ok = expect(partyhelpers::ClassifyDelMemberPath(false) == del_path::NON_LEADER, "del non-leader") && ok;
    ok = expect(partyhelpers::ShouldReloadPartyAfterLeaderDel(true), "reload after del") && ok;
    ok = expect(!partyhelpers::ShouldReloadPartyAfterLeaderDel(false), "no reload disband") && ok;

    ok = expect(partyhelpers::ShouldDeleteEmptyPartyOnPop(true), "delete empty") && ok;
    ok = expect(!partyhelpers::ShouldDeleteEmptyPartyOnPop(false), "keep non-empty") && ok;
    ok = expect(partyhelpers::ShouldClearAllianceMainOnPop(true, true), "clear main") && ok;
    ok = expect(!partyhelpers::ShouldClearAllianceMainOnPop(true, false), "not main") && ok;
    ok = expect(!partyhelpers::ShouldClearAllianceMainOnPop(false, true), "no alliance") && ok;

    // PushMember
    ok = expect(partyhelpers::ClassifyPushMember(true, false) == push_gate::REJECT_NULL_OR_HAS_PARTY, "push null") && ok;
    ok = expect(partyhelpers::ClassifyPushMember(false, true) == push_gate::REJECT_NULL_OR_HAS_PARTY, "push has party") && ok;
    ok = expect(partyhelpers::ClassifyPushMember(false, false) == push_gate::PROCEED, "push ok") && ok;
    ok = expect(partyhelpers::FormatPushMemberNullWarning() ==
                    "CParty::PushMember() - PEntity was null, or PParty not null.",
                "push warn") &&
         ok;

    ok = expect(partyhelpers::PartySyncFlag == 0x0100, "sync flag") && ok;
    ok = expect(partyhelpers::ShouldAssignLeaderFromFlags(0x0004), "assign leader") && ok;
    ok = expect(!partyhelpers::ShouldAssignLeaderFromFlags(0x0000), "no leader") && ok;
    ok = expect(partyhelpers::ShouldAssignQuarterMasterFromFlags(0x0010), "assign qm") && ok;
    ok = expect(partyhelpers::ShouldAssignSyncTargetFromFlags(0x0100), "assign sync") && ok;
    ok = expect(partyhelpers::ShouldAssignLeaderFromFlags(0x0114), "combined leader") && ok;
    ok = expect(partyhelpers::MemberInfoMatchesEntity(42, 42), "id match") && ok;
    ok = expect(!partyhelpers::MemberInfoMatchesEntity(42, 7), "id mismatch") && ok;

    return ok;
}
