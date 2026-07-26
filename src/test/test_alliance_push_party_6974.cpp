#include "test_alliance_push_party_6974.h"

#include "map/alliance.h"
#include "map/party.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance pushParty 6974 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CAlliance::pushParty characterization (slice 6974). It attaches a
// supplied party and appends it to the local list without the addParty
// duplicate/full admission path.
auto runAlliancePushParty6974SelfTests() -> bool
{
    CAlliance alliance(1);
    CParty    party(2);

    alliance.pushParty(&party, 2);

    return expect(party.m_PAlliance == &alliance, "party is attached to alliance") &&
           expect(alliance.partyList.size() == 1 && alliance.partyList.front() == &party, "party is appended to list");
}
