#include "test_party_add_member_host_7003.h"

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
        std::cerr << "party add member host 7003 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::AddMember characterization (slice 7003). A valid entity is
// attached and appended even when its default entity ID is zero; the next call
// rejects the same in-list entity before a second append.
auto runPartyAddMemberHost7003SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity member;
    party.m_PartyType = PARTY_MOBS;

    party.AddMember(&member);
    const bool firstAdd = expect(member.PParty == &party, "member attached") &&
                          expect(party.members.size() == 1, "member appended") &&
                          expect(party.members[0] == &member, "member retained");

    party.AddMember(&member);

    return firstAdd && expect(party.members.size() == 1, "duplicate member rejected");
}
