#include "test_party_member_lookup_1348.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "party member lookup 1348 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runPartyMemberLookup1348SelfTests() -> bool
{
    using get_gate    = partyhelpers::get_member_by_name_gate;
    using leader_gate = partyhelpers::set_leader_lookup_gate;
    bool ok           = true;

    ok = expect(partyhelpers::ClassifyGetMemberByName(false, false) == get_gate::REJECT_MOB_PARTY, "mob") && ok;
    ok = expect(partyhelpers::ClassifyGetMemberByName(true, true) == get_gate::REJECT_EMPTY, "empty") && ok;
    ok = expect(partyhelpers::ClassifyGetMemberByName(true, false) == get_gate::SEARCH, "search") && ok;

    ok = expect(partyhelpers::FormatGetMemberMobPartyWarning("Alice") ==
                    "Attempting to get Member data for Alice in Mob Party.",
                "mob warn") &&
         ok;

    ok = expect(partyhelpers::MemberNameMatches("Alice", "alice"), "case fold") && ok;
    ok = expect(partyhelpers::MemberNameMatches("Bob", "Bob"), "exact") && ok;
    ok = expect(!partyhelpers::MemberNameMatches("Alice", "Alic"), "len") && ok;
    ok = expect(!partyhelpers::MemberNameMatches("Alice", "Alicx"), "diff") && ok;
    ok = expect(partyhelpers::MemberNameMatches("", ""), "empty names") && ok;

    ok = expect(partyhelpers::ClassifySetLeaderLookup(true, true) == leader_gate::FOUND, "leader found") && ok;
    ok = expect(partyhelpers::ClassifySetLeaderLookup(true, false) == leader_gate::NOT_FOUND, "leader no row") && ok;
    ok = expect(partyhelpers::ClassifySetLeaderLookup(false, true) == leader_gate::NOT_FOUND, "leader query fail") && ok;

    ok = expect(partyhelpers::PartyLeaderFlag == 0x0004, "party leader flag") && ok;
    ok = expect(partyhelpers::AllianceLeaderFlag == 0x0008, "alliance leader flag") && ok;
    ok = expect(partyhelpers::ClearLeaderFlagsMask == 0x000C, "clear mask") && ok;
    ok = expect(partyhelpers::LeaderPartyFlags(true) == 0x000C, "flags both") && ok;
    ok = expect(partyhelpers::LeaderPartyFlags(false) == 0x0004, "flags party only") && ok;

    ok = expect(partyhelpers::ShouldRewriteAllianceIDOnLeaderChange(true, true), "rewrite yes") && ok;
    ok = expect(!partyhelpers::ShouldRewriteAllianceIDOnLeaderChange(true, false), "rewrite no id") && ok;
    ok = expect(!partyhelpers::ShouldRewriteAllianceIDOnLeaderChange(false, true), "rewrite no alliance") && ok;

    ok = expect(partyhelpers::NewPartyIDFromLeaderChar(99) == 99, "new party id") && ok;
    ok = expect(partyhelpers::ShouldUseMobPartyFirstMemberAsLeader(true), "mob first") && ok;
    ok = expect(!partyhelpers::ShouldUseMobPartyFirstMemberAsLeader(false), "pc not mob first") && ok;

    ok = expect(partyhelpers::ShouldQueryPartyInfo(true), "query pc") && ok;
    ok = expect(!partyhelpers::ShouldQueryPartyInfo(false), "query mob") && ok;
    ok = expect(partyhelpers::FormatGetPartyInfoMobWarning() == "Attempting to get Party data for Mob Party.", "party info warn") && ok;
    ok = expect(partyhelpers::GetPartyInfoAllianceIDInject(false, 5) == 0, "inject no alliance") && ok;
    ok = expect(partyhelpers::GetPartyInfoAllianceIDInject(true, 5) == 5, "inject alliance") && ok;
    ok = expect(partyhelpers::GetPartyInfoOrderFlags == 0x0003, "order flags") && ok;

    return ok;
}
