#include "test_party_push_member_host_7002.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"
#include "map/treasure_pool.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party push member host 7002 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::PushMember characterization (slice 7002). A valid entity is
// attached and appended. The source has no duplicate roster check: if a stale
// entity party pointer is cleared, the same entity is appended again. It then
// reloads the pushed character's treasure pool.
auto runPartyPushMemberHost7002SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity member;
    party.m_PartyType = PARTY_MOBS;

    party.PushMember(&member);
    const bool firstPush = expect(member.PParty == &party, "member attached") &&
                           expect(party.members.size() == 1, "member appended");

    member.PParty = nullptr;
    party.PushMember(&member);

    CTreasurePool oldPool(TreasurePoolType::Solo);
    CTreasurePool candidatePool(TreasurePoolType::Solo);
    CParty        pcParty(2);
    CCharEntity   pushed;
    CCharEntity   candidate;
    pushed.loc.destination    = 42;
    candidate.loc.destination = 42;
    pushed.PTreasurePool      = &oldPool;
    candidate.PTreasurePool   = &candidatePool;
    oldPool.addMember(&pushed);
    candidatePool.addMember(&candidate);
    candidate.PParty = &pcParty;
    pcParty.members.emplace_back(&candidate);
    pcParty.PushMember(&pushed);

    const bool reloadsPool = expect(pushed.PParty == &pcParty, "pushed character attached") &&
                             expect(pushed.PTreasurePool == &candidatePool, "pushed character pool reloaded") &&
                             expect(!oldPool.isMember(&pushed) && candidatePool.isMember(&pushed), "pushed character moves to candidate pool");

    pushed.PTreasurePool    = nullptr;
    candidate.PTreasurePool = nullptr;
    pushed.PParty           = nullptr;
    candidate.PParty        = nullptr;
    pcParty.members.clear();
    return firstPush &&
           expect(member.PParty == &party, "member reattached") &&
           expect(party.members.size() == 2, "stale member appended again") &&
           expect(party.members[0] == &member && party.members[1] == &member, "duplicate roster retained") &&
           reloadsPool;
}
