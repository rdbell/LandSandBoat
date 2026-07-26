#include "test_alliance_add_party_host_6991.h"

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
        std::cerr << "alliance add party host 6991 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CAlliance::addParty characterization (slice 6991). It attaches an
// admitted party, assigns the first slot when no persisted slot exists, and
// rejects duplicate/full additions without attaching the rejected party.
auto runAllianceAddPartyHost6991SelfTests() -> bool
{
    CAlliance alliance(1);
    CParty    first(10);
    CParty    second(20);
    CParty    third(30);
    CParty    fourth(40);

    alliance.addParty(&first);
    const bool firstAdded = expect(alliance.partyList.size() == 1, "first party added") &&
                            expect(first.m_PAlliance == &alliance, "first party attached") &&
                            expect(first.m_PartyNumber == 0, "first party receives main slot");

    alliance.addParty(&first);
    const bool duplicateRejected = expect(alliance.partyList.size() == 1, "duplicate not appended");

    alliance.addParty(&second);
    alliance.addParty(&third);
    alliance.addParty(&fourth);
    const bool fullRejected = expect(alliance.partyList.size() == 3, "alliance capacity preserved") &&
                              expect(fourth.m_PAlliance == nullptr, "full rejection leaves party detached");

    return firstAdded && duplicateRejected && fullRejected;
}
