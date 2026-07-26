#include "test_party_set_mob_leader_host_6998.h"

#include <iostream>
#include <stdexcept>

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
        std::cerr << "party set mob leader host 6998 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::SetLeader characterization (slice 6998). Mob parties ignore
// the requested name and select members.at(0), preserving its empty-roster
// std::out_of_range failure mode.
auto runPartySetMobLeaderHost6998SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity first;
    CBattleEntity second;
    party.m_PartyType = PARTY_MOBS;
    party.members     = { &first, &second };

    party.SetLeader("ignored");
    const bool firstSelected = expect(party.GetLeader() == &first, "first member selected");

    CParty emptyParty(2);
    emptyParty.m_PartyType = PARTY_MOBS;
    bool threw             = false;
    try
    {
        emptyParty.SetLeader("ignored");
    }
    catch (const std::out_of_range&)
    {
        threw = true;
    }

    return firstSelected && expect(threw, "empty roster throws");
}
