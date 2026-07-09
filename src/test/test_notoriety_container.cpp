/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_notoriety_container.h"

#include "map/enmity_container.h"
#include "map/entities/battle_entity.h"
#include "map/entities/mob_entity.h"
#include "map/notoriety_container.h"

#include <algorithm>
#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "notoriety container self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectSize(std::size_t actual, std::size_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "notoriety container self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto contains(CNotorietyContainer& container, CBattleEntity* entity) -> bool
{
    return std::find(container.begin(), container.end(), entity) != container.end();
}

auto testMembershipFilteringDeduplicationAndRemoval() -> bool
{
    CBattleEntity owner;
    owner.allegiance = ALLEGIANCE_TYPE::PLAYER;

    CBattleEntity sameAllegiance;
    sameAllegiance.allegiance = ALLEGIANCE_TYPE::PLAYER;

    CBattleEntity opponentA;
    opponentA.id         = 300;
    opponentA.allegiance = ALLEGIANCE_TYPE::MOB;

    CBattleEntity opponentB;
    opponentB.id         = 300;
    opponentB.allegiance = ALLEGIANCE_TYPE::MOB;

    CNotorietyContainer container(&owner);
    container.add(nullptr);
    container.add(&sameAllegiance);
    container.add(&opponentA);
    container.add(&opponentA);
    container.add(&opponentB);

    bool ok = true;
    ok      = expectSize(container.size(), 2, "opposing pointer-identity membership and deduplication") && ok;
    ok      = expectBool(contains(container, &sameAllegiance), false, "same-allegiance entity rejected") && ok;
    ok      = expectBool(contains(container, &opponentA), true, "first opposing entity accepted") && ok;
    ok      = expectBool(contains(container, &opponentB), true, "same-ID distinct pointer accepted") && ok;

    container.remove(nullptr);
    container.remove(&sameAllegiance);
    container.remove(&opponentA);
    ok = expectSize(container.size(), 1, "remove present entity") && ok;
    ok = expectBool(contains(container, &opponentA), false, "removed entity absent") && ok;
    container.remove(&opponentA);
    ok = expectSize(container.size(), 1, "repeated removal ignored") && ok;

    CNotorietyContainer noOwner(nullptr);
    noOwner.add(&opponentA);
    noOwner.remove(&opponentA);
    ok = expectSize(noOwner.size(), 0, "nil owner cannot acquire entries") && ok;
    ok = expectBool(noOwner.hasEnmity(), false, "nil owner has no enmity") && ok;
    return ok;
}

void addOwnerEnmity(CMobEntity& mob, CBattleEntity& owner, uint32 ownerID)
{
    mob.PEnmityContainer->GetEnmityList()->emplace(ownerID, EnmityObject_t{ &owner, 1, 1, true });
}

auto testHasEnmityPrunesStaleMobsAndRetainsOtherEntities() -> bool
{
    CBattleEntity owner;
    owner.id         = 0x12345;
    owner.allegiance = ALLEGIANCE_TYPE::PLAYER;
    const auto ownerID = static_cast<uint16>(owner.id);

    CMobEntity deadWithEnmity;
    deadWithEnmity.health.hp = 0;
    deadWithEnmity.status    = STATUS_TYPE::UPDATE;
    addOwnerEnmity(deadWithEnmity, owner, ownerID);

    CMobEntity liveMissingOwner;
    liveMissingOwner.health.hp = 1;
    liveMissingOwner.status    = STATUS_TYPE::UPDATE;
    addOwnerEnmity(liveMissingOwner, owner, static_cast<uint16>(ownerID + 1));

    CMobEntity liveWithOwner;
    liveWithOwner.health.hp = 1;
    liveWithOwner.status    = STATUS_TYPE::UPDATE;
    addOwnerEnmity(liveWithOwner, owner, ownerID);

    CBattleEntity nonMob;
    nonMob.allegiance = ALLEGIANCE_TYPE::MOB;

    CNotorietyContainer container(&owner);
    container.add(&deadWithEnmity);
    container.add(&liveMissingOwner);
    container.add(&liveWithOwner);
    container.add(&nonMob);

    bool ok = true;
    ok      = expectBool(container.hasEnmity(), true, "retained entries report enmity") && ok;
    ok      = expectSize(container.size(), 2, "dead and missing-owner mobs pruned") && ok;
    ok      = expectBool(contains(container, &deadWithEnmity), false, "dead mob removed despite owner enmity") && ok;
    ok      = expectBool(contains(container, &liveMissingOwner), false, "live mob missing owner removed") && ok;
    ok      = expectBool(contains(container, &liveWithOwner), true, "live mob retaining owner remains") && ok;
    ok      = expectBool(contains(container, &nonMob), true, "non-mob battle entity retained") && ok;

    container.remove(&nonMob);
    ok = expectBool(container.hasEnmity(), true, "live owner-enmity mob reports enmity") && ok;

    liveWithOwner.PEnmityContainer->GetEnmityList()->erase(ownerID);
    ok = expectBool(container.hasEnmity(), false, "live mob loses owner enmity") && ok;
    ok = expectSize(container.size(), 0, "final stale mob pruned") && ok;
    return ok;
}

} // namespace

auto runNotorietyContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testMembershipFilteringDeduplicationAndRemoval() && ok;
    ok      = testHasEnmityPrunesStaleMobsAndRetainsOtherEntities() && ok;
    return ok;
}
