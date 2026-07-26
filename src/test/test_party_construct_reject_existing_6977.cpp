#include "test_party_construct_reject_existing_6977.h"

#include "map/entities/battle_entity.h"
#include "map/party.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party construct-reject-existing 6977 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CParty(CBattleEntity*) rejection characterization (slice 6977). An
// entity that already owns a party does not seed another party instance.
auto runPartyConstructRejectExisting6977SelfTests() -> bool
{
    CBattleEntity entity;
    entity.id      = 77;
    entity.objtype = TYPE_MOB;

    CParty existing(&entity);
    CParty rejected(&entity);

    const bool existingPartyRemainsAttached = entity.PParty == &existing;
    const bool rejectedRetainsDefaultID     = rejected.GetPartyID() == 0;
    const bool rejectedHasNoLeader          = rejected.GetLeader() == nullptr;
    const bool rejectedHasNoMembers         = rejected.members.empty();

    entity.PParty = nullptr;

    return expect(existingPartyRemainsAttached, "existing party remains attached") &&
           expect(rejectedRetainsDefaultID, "rejected constructor retains default ID") &&
           expect(rejectedHasNoLeader, "rejected constructor has no leader") &&
           expect(rejectedHasNoMembers, "rejected constructor has no members");
}
