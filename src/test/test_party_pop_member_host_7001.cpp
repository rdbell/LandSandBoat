#include "test_party_pop_member_host_7001.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/alliance.h"
#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party pop member host 7001 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::PopMember characterization (slice 7001). Removing a member
// from a nonempty party retains the remaining roster and detaches only the
// popped entity without changing existing party roles. A final pop removes
// the party from its alliance and clears the main-party pointer before delete.
auto runPartyPopMemberHost7001SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity leaving;
    CBattleEntity remaining;
    leaving.PParty   = &party;
    remaining.PParty = &party;
    party.members    = { &leaving, &remaining };
    party.m_PLeader  = &remaining;

    party.PopMember(&leaving);

    const bool nonEmptyPop = expect(leaving.PParty == nullptr, "popped member detached") &&
                             expect(remaining.PParty == &party, "remaining member stays attached") &&
                             expect(party.members.size() == 1, "member removed") &&
                             expect(party.members[0] == &remaining, "remaining roster preserved") &&
                             expect(party.GetLeader() == &remaining, "leader retained");

    CAlliance     alliance(2);
    auto*          finalParty = new CParty(2);
    CBattleEntity finalMember;
    finalParty->m_PAlliance = &alliance;
    finalParty->m_PLeader   = &finalMember;
    finalParty->members     = { &finalMember };
    finalMember.PParty      = finalParty;
    alliance.setMainParty(finalParty);
    alliance.partyList.emplace_back(finalParty);

    finalParty->PopMember(&finalMember);

    const bool finalPop = expect(finalMember.PParty == nullptr, "final member detached") &&
                          expect(alliance.getMainParty() == nullptr, "alliance main party cleared") &&
                          expect(alliance.partyList.empty(), "party removed from alliance list");
    return nonEmptyPop && finalPop;
}
