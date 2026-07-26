#include "test_party_disband_mob_host_7000.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/alliance.h"
#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"

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

// Direct CParty::DisbandParty characterization. The mob branch detaches every
// roster entity before deleting the party; the common prelude detaches an
// attached alliance before the PC branch's final deletion.
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

    const bool mobDisband = expect(first.PParty == nullptr, "first member detached") &&
                            expect(second.PParty == nullptr, "second member detached") &&
                            expect(!removeReturned, "singleton leader removal returns false") &&
                            expect(singleton.PParty == nullptr, "singleton leader removal disbands");

    CAlliance alliance(3);
    auto*     pcParty = new CParty(3);
    pcParty->m_PAlliance = &alliance;
    alliance.partyList.emplace_back(pcParty);

    pcParty->DisbandParty(false);

    const bool pcPrelude = expect(alliance.partyList.empty(), "PC disband detaches alliance before deletion");

    auto*       memberParty = new CParty(4);
    CCharEntity pcMember;
    pcMember.PParty       = memberParty;
    memberParty->members  = { &pcMember };
    memberParty->m_PLeader = &pcMember;
    memberParty->DisbandParty(false);

    const bool pcMemberDisband = expect(pcMember.PParty == nullptr, "PC disband detaches member") &&
                                 expect(pcMember.ReloadParty(), "PC disband clears trusts and requests reload");
    return mobDisband && pcPrelude && pcMemberDisband;
}
