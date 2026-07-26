#include "test_party_remove_mob_leader_host_6999.h"

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
        std::cerr << "party remove mob leader host 6999 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::RemovePartyLeader characterization (slice 6999). Removing a
// mob-party leader promotes the first different member, then removes the old
// leader, preserving the remaining member order.
auto runPartyRemoveMobLeaderHost6999SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity leaving;
    CBattleEntity next;
    CBattleEntity later;
    party.m_PartyType = PARTY_MOBS;
    party.members     = { &leaving, &next, &later };
    party.m_PLeader   = &leaving;

    const bool removed = party.RemovePartyLeader(&leaving);

    return expect(removed, "returns true") &&
           expect(party.GetLeader() == &next, "first remaining member promoted") &&
           expect(party.members.size() == 2, "leader removed") &&
           expect(party.members[0] == &next, "next member remains first") &&
           expect(party.members[1] == &later, "later member remains second");
}
