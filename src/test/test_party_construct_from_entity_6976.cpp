#include "test_party_construct_from_entity_6976.h"

#include "map/entities/battle_entity.h"
#include "map/party.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party construct-from-entity 6976 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CParty(CBattleEntity*) characterization (slice 6976). The mob path
// has no persistence dependencies: it creates a mob party led by and
// containing the supplied entity.
auto runPartyConstructFromEntity6976SelfTests() -> bool
{
    CBattleEntity leader;
    leader.id      = 77;
    leader.objtype = TYPE_MOB;

    CParty party(&leader);

    CBattleEntity second;
    CBattleEntity third;
    CBattleEntity fourth;
    CBattleEntity fifth;
    CBattleEntity sixth;
    CBattleEntity seventh;
    party.AddMember(&second);
    party.AddMember(&third);
    party.AddMember(&fourth);
    party.AddMember(&fifth);
    party.AddMember(&sixth);
    party.AddMember(&seventh);

    const bool partyIDComesFromEntity = party.GetPartyID() == 77;
    const bool entityIsLeader         = party.GetLeader() == &leader;
    const bool leaderIsAttached       = leader.PParty == &party;
    const bool mobPartyHasNoCapacity  = party.members.size() == 7;

    for (auto* member : party.members)
    {
        member->PParty = nullptr;
    }
    party.members.clear();

    return expect(partyIDComesFromEntity, "party ID comes from entity") &&
           expect(entityIsLeader, "entity is leader") &&
           expect(leaderIsAttached, "leader is attached") &&
           expect(mobPartyHasNoCapacity, "mob party does not enforce PC capacity");
}
