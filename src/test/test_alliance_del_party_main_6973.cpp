#include "test_alliance_del_party_main_6973.h"

#include "map/alliance.h"
#include "map/party.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance delParty main 6973 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CAlliance::delParty characterization (slice 6973). Low-level
// deletion removes the party but deliberately does not rewrite aLeader; the
// higher-level removeParty path is responsible for promotion or dissolution.
auto runAllianceDelPartyMain6973SelfTests() -> bool
{
    CAlliance alliance(1);
    CParty    party(1);
    party.m_PAlliance = &alliance;
    alliance.partyList.emplace_back(&party);
    alliance.setMainParty(&party);

    alliance.delParty(&party);

    return expect(alliance.partyList.empty(), "party is removed from list") &&
           expect(alliance.getMainParty() == &party, "low-level delete preserves main party pointer") &&
           expect(party.m_PAlliance == nullptr, "party is detached from alliance");
}
