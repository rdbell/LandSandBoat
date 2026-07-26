#include "test_alliance_del_party_host_6992.h"

#include <iostream>

#define private public
#include "map/alliance.h"
#include "map/party.h"
#undef private

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance del party host 6992 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CAlliance::delParty characterization (slice 6992). Removing an
// attached party erases it, resets its slot, and intentionally leaves the
// alliance main-party pointer unchanged for the higher-level workflow.
auto runAllianceDelPartyHost6992SelfTests() -> bool
{
    CAlliance alliance(1);
    CParty    first(10);
    CParty    second(20);

    second.m_PartyType = PARTY_MOBS; // avoids unrelated PC reload host work
    alliance.addParty(&first);
    alliance.addParty(&second);
    first.m_PartyNumber = 2;
    alliance.setMainParty(&first);

    alliance.delParty(&first);
    const bool detached = expect(alliance.partyList.size() == 1, "party erased") &&
                          expect(first.m_PAlliance == nullptr, "party detached") &&
                          expect(first.m_PartyNumber == 0, "party number reset") &&
                          expect(alliance.getMainParty() == &first, "main party intentionally preserved");

    alliance.delParty(&first);
    const bool repeatedSkip = expect(alliance.partyList.size() == 1, "detached party second delete skipped");

    return detached && repeatedSkip;
}
