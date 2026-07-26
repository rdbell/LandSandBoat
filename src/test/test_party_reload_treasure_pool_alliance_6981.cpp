#include "test_party_reload_treasure_pool_alliance_6981.h"

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
        std::cerr << "party reload treasure pool alliance 6981 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::ReloadTreasurePool characterization (slice 6981). An
// alliance member scans local alliance parties and their members in vector
// order, joining the first eligible same-zone member's pool.
auto runPartyReloadTreasurePoolAlliance6981SelfTests() -> bool
{
    CTreasurePool oldPool(TreasurePoolType::Solo);
    CTreasurePool firstPool(TreasurePoolType::Solo);
    CTreasurePool laterPool(TreasurePoolType::Solo);
    CCharEntity   self;
    CCharEntity   first;
    CCharEntity   later;
    CParty        ownParty(1);
    CParty        firstParty(2);
    CParty        laterParty(3);
    CAlliance     alliance(1);

    self.loc.destination  = 42;
    first.loc.destination = 42;
    later.loc.destination = 42;

    self.PParty  = &ownParty;
    first.PParty = &firstParty;
    later.PParty = &laterParty;
    ownParty.members.emplace_back(&self);
    firstParty.members.emplace_back(&first);
    laterParty.members.emplace_back(&later);
    ownParty.m_PAlliance   = &alliance;
    firstParty.m_PAlliance = &alliance;
    laterParty.m_PAlliance = &alliance;
    alliance.partyList.emplace_back(&ownParty);
    alliance.partyList.emplace_back(&firstParty);
    alliance.partyList.emplace_back(&laterParty);

    self.PTreasurePool = &oldPool;
    oldPool.addMember(&self);
    first.PTreasurePool = &firstPool;
    firstPool.addMember(&first);
    later.PTreasurePool = &laterPool;
    laterPool.addMember(&later);

    ownParty.ReloadTreasurePool(&self);

    const bool ok = expect(self.PTreasurePool == &firstPool, "self joins first alliance candidate pool") &&
                    expect(firstPool.isMember(&self) && firstPool.isMember(&first), "first pool has both members") &&
                    expect(firstPool.getPoolType() == TreasurePoolType::Alliance, "joined party pool promotes to alliance") &&
                    expect(!oldPool.isMember(&self), "old pool removes self") &&
                    expect(!laterPool.isMember(&self), "later alliance candidate is not selected");

    self.PTreasurePool  = nullptr;
    first.PTreasurePool = nullptr;
    later.PTreasurePool = nullptr;
    self.PParty         = nullptr;
    first.PParty        = nullptr;
    later.PParty        = nullptr;
    ownParty.m_PAlliance   = nullptr;
    firstParty.m_PAlliance = nullptr;
    laterParty.m_PAlliance = nullptr;
    return ok;
}
