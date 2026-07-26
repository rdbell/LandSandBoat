#include "test_party_remove_member_mob_leader_host_7007.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party remove member mob leader host 7007 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::RemoveMember characterization (slice 7007). A mob leader
// follows RemovePartyLeader: it promotes the first other member, then detaches
// and erases the former leader through the non-PC removal path.
auto runPartyRemoveMemberMobLeaderHost7007SelfTests() -> bool
{
    CParty        party(1);
    CCharEntity   leaving;
    CBattleEntity next;
    CBattleEntity later;
    party.m_PartyType = PARTY_MOBS;
    party.m_PLeader   = &leaving;
    party.members     = { &leaving, &next, &later };
    leaving.PParty    = &party;
    next.PParty       = &party;
    later.PParty      = &party;

    party.RemoveMember(&leaving);

    return expect(leaving.PParty == nullptr, "former leader detached") &&
           expect(leaving.ReloadParty(), "character leader clears trusts and requests reload") &&
           expect(next.PParty == &party, "successor remains attached") &&
           expect(later.PParty == &party, "other member remains attached") &&
           expect(party.GetLeader() == &next, "first other member promoted") &&
           expect(party.members.size() == 2, "former leader erased") &&
           expect(party.members[0] == &next, "successor remains first") &&
           expect(party.members[1] == &later, "later member retained");
}
