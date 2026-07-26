#include "test_party_remove_member_host_7005.h"

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
        std::cerr << "party remove member host 7005 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::RemoveMember characterization (slice 7005). The non-leader
// mob path erases and detaches the selected member without touching the leader.
auto runPartyRemoveMemberHost7005SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity leader;
    CBattleEntity leaving;
    party.m_PartyType = PARTY_MOBS;
    party.m_PLeader   = &leader;
    party.members     = { &leader, &leaving };
    leader.PParty     = &party;
    leaving.PParty    = &party;

    party.RemoveMember(&leaving);

    return expect(leaving.PParty == nullptr, "leaving member detached") &&
           expect(leader.PParty == &party, "leader remains attached") &&
           expect(party.members.size() == 1, "leaving member erased") &&
           expect(party.members[0] == &leader, "leader remains in roster") &&
           expect(party.GetLeader() == &leader, "leader retained");
}
