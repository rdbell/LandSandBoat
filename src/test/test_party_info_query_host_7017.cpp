#include "test_party_info_query_host_7017.h"

#include <iostream>

#include "map/party_capacity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party info query host 7017 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// CParty::GetPartyInfo characterization (slice 7017). Only PC parties query
// accounts_parties; the plan preserves query binding order and alliance scope.
auto runPartyInfoQueryHost7017SelfTests() -> bool
{
    const auto mob = partyhelpers::PlanGetPartyInfoQuery(false, true, 77, 42);
    const auto party = partyhelpers::PlanGetPartyInfoQuery(true, false, 77, 42);
    const auto alliance = partyhelpers::PlanGetPartyInfoQuery(true, true, 77, 42);

    return expect(!mob.query && mob.allianceID == 0 && mob.partyID == 0 && mob.orderFlags == 0, "mob skips query") &&
           expect(party.query && party.allianceID == 0 && party.partyID == 42 && party.orderFlags == 0x0003,
                  "PC party binding plan") &&
           expect(alliance.query && alliance.allianceID == 77 && alliance.partyID == 42 && alliance.orderFlags == 0x0003,
                  "PC alliance binding plan");
}
