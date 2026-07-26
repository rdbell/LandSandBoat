#include "test_party_push_member_host_7002.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party push member host 7002 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::PushMember characterization (slice 7002). A valid entity is
// attached and appended. The source has no duplicate roster check: if a stale
// entity party pointer is cleared, the same entity is appended again.
auto runPartyPushMemberHost7002SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity member;
    party.m_PartyType = PARTY_MOBS;

    party.PushMember(&member);
    const bool firstPush = expect(member.PParty == &party, "member attached") &&
                           expect(party.members.size() == 1, "member appended");

    member.PParty = nullptr;
    party.PushMember(&member);

    return firstPush &&
           expect(member.PParty == &party, "member reattached") &&
           expect(party.members.size() == 2, "stale member appended again") &&
           expect(party.members[0] == &member && party.members[1] == &member, "duplicate roster retained");
}
