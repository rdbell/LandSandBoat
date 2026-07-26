#include "test_party_set_quartermaster_host_6990.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party set quartermaster host 6990 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::SetQuarterMaster characterization (slice 6990). The named
// local member becomes quartermaster; a missed lookup clears the pointer.
auto runPartySetQuarterMasterHost6990SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity alice;
    CBattleEntity bob;

    alice.name = "Alice";
    bob.name   = "Bob";
    party.members = { &alice, &bob };

    party.SetQuarterMaster("bOb");
    const bool assigned = expect(party.GetQuaterMaster() == &bob, "case-insensitive member assigned");

    party.SetQuarterMaster("Carol");
    const bool cleared = expect(party.GetQuaterMaster() == nullptr, "unknown member clears quartermaster");

    return assigned && cleared;
}
