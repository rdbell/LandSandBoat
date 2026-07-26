#include "test_alliance_push_party_6974.h"

#include "map/alliance.h"
#include "map/entities/char_entity.h"
#include "map/party.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "alliance pushParty 6974 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CAlliance::pushParty characterization (slice 6974). It attaches a
// supplied party and appends it to the local list without the addParty
// duplicate/full admission path, then reloads every character member's
// treasure pool before saving its character stats.
auto runAlliancePushParty6974SelfTests() -> bool
{
    CTreasurePool oldPool(TreasurePoolType::Solo);
    CTreasurePool candidatePool(TreasurePoolType::Solo);
    CCharEntity   self;
    CCharEntity   candidate;
    CAlliance     alliance(1);
    CParty        party(2);

    self.loc.destination      = 42;
    candidate.loc.destination = 42;
    self.PParty               = &party;
    candidate.PParty          = &party;
    party.members.emplace_back(&self);
    party.members.emplace_back(&candidate);

    self.PTreasurePool = &oldPool;
    oldPool.addMember(&self);
    candidate.PTreasurePool = &candidatePool;
    candidatePool.addMember(&candidate);

    alliance.pushParty(&party, 2);

    const bool ok = expect(party.m_PAlliance == &alliance, "party is attached to alliance") &&
                    expect(alliance.partyList.size() == 1 && alliance.partyList.front() == &party, "party is appended to list") &&
                    expect(self.PTreasurePool == &candidatePool, "member treasure pool is reloaded") &&
                    expect(!oldPool.isMember(&self) && candidatePool.isMember(&self), "member moves to candidate pool");

    self.PTreasurePool      = nullptr;
    candidate.PTreasurePool = nullptr;
    self.PParty             = nullptr;
    candidate.PParty        = nullptr;
    party.m_PAlliance       = nullptr;
    party.members.clear();
    return ok;
}
