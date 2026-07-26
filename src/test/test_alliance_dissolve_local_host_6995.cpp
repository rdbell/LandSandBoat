#include "test_alliance_dissolve_local_host_6995.h"

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
        std::cerr << "alliance dissolve local host 6995 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CAlliance::dissolveAlliance(false) characterization (slice 6995).
// The server branch detaches every local party, clears its alliance slot, and
// then destroys the alliance object.
auto runAllianceDissolveLocalHost6995SelfTests() -> bool
{
    auto* alliance = new CAlliance(1);
    CParty first(10);
    CParty second(20);

    first.m_PartyType  = PARTY_MOBS; // avoids unrelated PC reload host work
    second.m_PartyType = PARTY_MOBS;
    first.m_PAlliance  = alliance;
    second.m_PAlliance = alliance;
    first.m_PartyNumber = 1;
    second.m_PartyNumber = 2;
    alliance->partyList = { &first, &second };
    alliance->setMainParty(&first);

    alliance->dissolveAlliance(false);

    return expect(first.m_PAlliance == nullptr, "first party detached") &&
           expect(second.m_PAlliance == nullptr, "second party detached") &&
           expect(first.m_PartyNumber == 0, "first party number reset") &&
           expect(second.m_PartyNumber == 0, "second party number reset");
}
