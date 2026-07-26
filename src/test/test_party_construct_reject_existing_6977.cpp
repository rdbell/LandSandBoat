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

    return expect(entity.PParty == &existing, "existing party remains attached") &&
           expect(rejected.GetPartyID() == 0, "rejected constructor retains default ID") &&
           expect(rejected.GetLeader() == nullptr, "rejected constructor has no leader") &&
           expect(rejected.members.empty(), "rejected constructor has no members");
}
