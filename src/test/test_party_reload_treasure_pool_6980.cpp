#include "test_party_reload_treasure_pool_6980.h"

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
        std::cerr << "party reload treasure pool 6980 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::ReloadTreasurePool characterization (slice 6980). A member
// leaves its non-zone pool and joins the first eligible same-zone party
// member's pool; unrelated members remain in the old pool.
auto runPartyReloadTreasurePool6980SelfTests() -> bool
{
    CTreasurePool oldPool(TreasurePoolType::Solo);
    CTreasurePool candidatePool(TreasurePoolType::Solo);
    CCharEntity   self;
    CCharEntity   candidate;
    CCharEntity   leftover;
    CParty        party(1);

    self.loc.destination      = 42;
    candidate.loc.destination = 42;

    self.PParty      = &party;
    candidate.PParty = &party;
    party.members.emplace_back(&self);
    party.members.emplace_back(&candidate);

    self.PTreasurePool = &oldPool;
    oldPool.addMember(&self);
    leftover.PTreasurePool = &oldPool;
    oldPool.addMember(&leftover);

    candidate.PTreasurePool = &candidatePool;
    candidatePool.addMember(&candidate);

    party.ReloadTreasurePool(&self);

    const bool ok = expect(self.PTreasurePool == &candidatePool, "self joins candidate pool") &&
                    expect(candidatePool.isMember(&self) && candidatePool.isMember(&candidate), "candidate pool has both members") &&
                    expect(!oldPool.isMember(&self) && oldPool.isMember(&leftover), "old pool retains only leftover member");

    self.PTreasurePool      = nullptr;
    candidate.PTreasurePool = nullptr;
    leftover.PTreasurePool  = nullptr;
    self.PParty             = nullptr;
    candidate.PParty        = nullptr;
    return ok;
}
