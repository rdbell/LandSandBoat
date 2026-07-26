#include "test_party_disband_mob_host_7000.h"

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
        std::cerr << "party disband mob host 7000 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::DisbandParty characterization (slice 7000). The mob branch
// detaches every roster entity from the party before deleting the party.
auto runPartyDisbandMobHost7000SelfTests() -> bool
{
    auto* party = new CParty(1);

    CBattleEntity first;
    CBattleEntity second;
    first.PParty = party;
    second.PParty = party;

    party->m_PartyType = PARTY_MOBS;
    party->members     = { &first, &second };
    party->m_PLeader   = &first;

    party->DisbandParty();

    auto* singletonParty = new CParty(2);
    CBattleEntity singleton;
    singleton.PParty           = singletonParty;
    singletonParty->m_PartyType = PARTY_MOBS;
    singletonParty->members     = { &singleton };
    singletonParty->m_PLeader   = &singleton;
    const bool removeReturned   = singletonParty->RemovePartyLeader(&singleton);

    return expect(first.PParty == nullptr, "first member detached") &&
           expect(second.PParty == nullptr, "second member detached") &&
           expect(!removeReturned, "singleton leader removal returns false") &&
           expect(singleton.PParty == nullptr, "singleton leader removal disbands");
}
