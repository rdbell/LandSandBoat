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

// Direct CParty::RemoveMember and CParty::DelMember characterization. Their
// non-leader mob paths erase and detach the selected member without touching
// the leader.
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

    const bool removeMember = expect(leaving.PParty == nullptr, "remove leaving member detached") &&
                              expect(leader.PParty == &party, "remove leader remains attached") &&
                              expect(party.members.size() == 1, "remove leaving member erased") &&
                              expect(party.members[0] == &leader, "remove leader remains in roster") &&
                              expect(party.GetLeader() == &leader, "remove leader retained");

    CParty        delParty(2);
    CBattleEntity delLeader;
    CBattleEntity delLeaving;
    delParty.m_PartyType = PARTY_MOBS;
    delParty.m_PLeader   = &delLeader;
    delParty.members     = { &delLeader, &delLeaving };
    delLeader.PParty     = &delParty;
    delLeaving.PParty    = &delParty;

    delParty.DelMember(&delLeaving);

    const bool delMember = expect(delLeaving.PParty == nullptr, "del leaving member detached") &&
                           expect(delLeader.PParty == &delParty, "del leader remains attached") &&
                           expect(delParty.members.size() == 1, "del leaving member erased") &&
                           expect(delParty.members[0] == &delLeader, "del leader remains in roster") &&
                           expect(delParty.GetLeader() == &delLeader, "del leader retained");
    return removeMember && delMember;
}
