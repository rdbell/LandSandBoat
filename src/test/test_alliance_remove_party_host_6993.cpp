#include "test_alliance_remove_party_host_6993.h"

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
        std::cerr << "alliance remove party host 6993 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CAlliance::removeParty characterization (slice 6993). A non-main
// party is detached through delParty while the alliance lead remains attached.
auto runAllianceRemovePartyHost6993SelfTests() -> bool
{
    CAlliance alliance(1);
    CParty    mainParty(10);
    CParty    leavingParty(20);

    mainParty.m_PartyType = PARTY_MOBS; // avoids unrelated PC reload host work
    alliance.addParty(&mainParty);
    alliance.addParty(&leavingParty);
    alliance.setMainParty(&mainParty);
    leavingParty.m_PartyNumber = 2;

    alliance.removeParty(&leavingParty);

    return expect(alliance.partyList.size() == 1, "leaving party erased") &&
           expect(alliance.partyList.front() == &mainParty, "main party remains") &&
           expect(leavingParty.m_PAlliance == nullptr, "leaving party detached") &&
           expect(leavingParty.m_PartyNumber == 0, "leaving party number reset") &&
           expect(alliance.getMainParty() == &mainParty, "main party retained");
}
