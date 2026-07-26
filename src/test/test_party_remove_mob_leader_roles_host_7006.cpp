#include "test_party_remove_mob_leader_roles_host_7006.h"

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
        std::cerr << "party remove mob leader roles host 7006 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::RemovePartyLeader characterization (slice 7006). Its mob
// promotion path calls DelMember after changing m_PLeader. As a non-PC leave,
// that DelMember path intentionally does not clear stale QM or sync pointers.
auto runPartyRemoveMobLeaderRolesHost7006SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity leaving;
    CBattleEntity next;
    party.m_PartyType       = PARTY_MOBS;
    party.members           = { &leaving, &next };
    party.m_PLeader         = &leaving;
    party.m_PQuarterMaster  = &leaving;
    party.m_PSyncTarget     = &leaving;

    const bool removed = party.RemovePartyLeader(&leaving);

    return expect(removed, "returns true") &&
           expect(party.GetLeader() == &next, "successor promoted") &&
           expect(party.m_PQuarterMaster == &leaving, "quartermaster retained") &&
           expect(party.m_PSyncTarget == &leaving, "sync target retained");
}
