#include "test_alliance_construct_from_party_6975.h"

#include "map/alliance.h"
#include "map/entities/char_entity.h"
#include "map/party.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance construct-from-party 6975 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CAlliance(CBattleEntity*) characterization (slice 6975). A valid
// entity party becomes the new alliance's sole local and main party.
auto runAllianceConstructFromParty6975SelfTests() -> bool
{
    CCharEntity leader;
    CParty      party(42);
    leader.PParty = &party;

    CAlliance alliance(&leader);

    return expect(alliance.m_AllianceID == 42, "alliance ID comes from party") &&
           expect(party.m_PAlliance == &alliance, "party is attached") &&
           expect(alliance.partyList.size() == 1 && alliance.partyList.front() == &party, "party is sole list entry") &&
           expect(alliance.getMainParty() == &party, "party is main party");
}
