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

#include "test_recast_container.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "map/char_recast_container.h"
#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"
#include "map/enums/loot_recast.h"
#include "map/recast_container.h"

#include <cstdint>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

using namespace std::chrono_literals;

namespace
{

class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>&) -> std::unique_ptr<db::ResultSet> override
    {
        return std::make_unique<db::LibMariaDBResultSet>(static_cast<std::size_t>(1), query);
    }

    auto getSchema() -> std::string override
    {
        return "xidb";
    }

    auto getVersion() -> std::string override
    {
        return "test";
    }

    auto getDriverVersion() -> std::string override
    {
        return "test";
    }
};

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "recast container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "recast container self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectSeconds(timer::duration actual, std::int64_t expected, const char* label) -> bool
{
    const auto seconds = timer::count_seconds(actual);
    if (seconds != expected)
    {
        std::cerr << "recast container self-test failed: " << label << " got "
                  << seconds << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testLoadAndLookup() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    auto* magic = container.Load(RECAST_MAGIC, Recast::BloodPactRage, 30s);
    auto* same  = container.Load(RECAST_MAGIC, Recast::BloodPactRage, 15s);

    bool ok = true;
    ok      = expectBool(magic == same, true, "Load returns existing recast") && ok;
    ok      = expectUInt(container.GetRecastList(RECAST_MAGIC)->size(), 1, "magic list size") && ok;
    ok      = expectBool(container.Has(RECAST_MAGIC, Recast::BloodPactRage), true, "Has magic recast") && ok;
    ok      = expectBool(container.Has(RECAST_MAGIC, Recast::BloodPactWard), false, "Has missing magic recast") && ok;
    ok      = expectSeconds(same->RecastTime, 15, "non-charge recast overwritten") && ok;
    return ok;
}

auto testChargedRecastAccumulation() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    auto* recast = container.Load(RECAST_ABILITY, Recast::Sic, 10s, 10s, 3);
    auto  initialTimeStamp = recast->TimeStamp;
    recast       = container.Load(RECAST_ABILITY, Recast::Sic, 10s, 10s, 3);
    recast       = container.Load(RECAST_ABILITY, Recast::Sic, 20s, 10s, 3);

    bool ok = true;
    ok      = expectSeconds(recast->RecastTime, 30, "charged recast cap") && ok;
    ok      = expectSeconds(recast->TimeStamp - initialTimeStamp, 10, "charged recast cap timestamp shift") && ok;
    ok      = expectSeconds(recast->chargeTime, 10, "charge time retained") && ok;
    ok      = expectUInt(recast->maxCharges, 3, "max charges retained") && ok;
    ok      = expectBool(container.HasRecast(RECAST_ABILITY, Recast::Sic, 1s), true, "one charge blocked") && ok;
    ok      = expectBool(container.HasRecast(RECAST_ABILITY, Recast::Sic, 4s), true, "too many charges blocked") && ok;
    return ok;
}

auto testMutationBoundaries() -> bool
{
    CCharEntity          character;
    CCharRecastContainer container(&character);

    // Recast is uint16: Mount (256) must remain distinct from Special (0),
    // including through the ability list's zero-in-place index deletion.
    container.Load(RECAST_ABILITY, Recast::Special, 1h);
    container.Load(RECAST_ABILITY, Recast::Mount, 20s, 10s, 3);
    container.Load(RECAST_ABILITY, Recast::Sic, 30s);
    const auto maxID = static_cast<Recast>(std::numeric_limits<std::uint16_t>::max());
    container.Load(RECAST_ABILITY, maxID, 40s);
    FakeDatabase fake;
    db::setDatabase(&fake);
    container.DeleteByIndex(RECAST_ABILITY, 1);
    db::setDatabase(nullptr);

    auto* abilities = container.GetRecastList(RECAST_ABILITY);
    bool  ok        = true;
    ok = expectUInt(abilities->size(), 4, "ability mutation list size") && ok;
    ok = expectBool(abilities->at(0).ID == Recast::Special, true, "ability mutation first ID") && ok;
    ok = expectBool(abilities->at(1).ID == Recast::Mount, true, "ability mutation uint16 ID") && ok;
    ok = expectSeconds(abilities->at(1).RecastTime, 0, "ability mutation zero in place") && ok;
    ok = expectBool(abilities->at(2).ID == Recast::Sic, true, "ability mutation preserves order") && ok;
    ok = expectBool(abilities->at(3).ID == maxID, true, "ability mutation uint16 maximum ID") && ok;

    // Load replaces nonzero charge metadata before applying the new cap.
    auto* mount = container.Load(RECAST_ABILITY, Recast::Mount, 10s, 10s, 3);
    auto  beforeCap = mount->TimeStamp;
    mount           = container.Load(RECAST_ABILITY, Recast::Mount, 5s, 5s, 2);
    ok = expectSeconds(mount->chargeTime, 5, "updated charge time") && ok;
    ok = expectUInt(mount->maxCharges, 2, "updated max charges") && ok;
    ok = expectSeconds(mount->RecastTime, 10, "updated charge cap") && ok;
    ok = expectSeconds(mount->TimeStamp - beforeCap, 5, "updated cap timestamp shift") && ok;

    // timer::count_seconds floors negative fractions instead of truncating
    // toward zero. Just above a one-charge recast therefore permits -500ms.
    mount->TimeStamp  = timer::now();
    mount->RecastTime = 11s;
    mount->chargeTime = 10s;
    mount->maxCharges = 1;
    ok = expectBool(container.HasRecast(RECAST_ABILITY, Recast::Mount, -500ms), false, "negative fractional charge request") && ok;
    ok = expectBool(container.HasRecast(RECAST_ABILITY, Recast::Mount, 999ms), true, "positive fractional charge request") && ok;

    // Check expires at equality and zeroes abilities in place.
    mount->TimeStamp = timer::now() - mount->RecastTime;
    container.Check();
    ok = expectBool(container.GetRecast(RECAST_ABILITY, Recast::Mount) != nullptr, true, "exact-expiry ability retained") && ok;
    ok = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Mount)->RecastTime, 0, "exact-expiry ability zeroed") && ok;
    return ok;
}

auto testRestoredAbilityRecast() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    // charutils restores a persisted row by passing its remaining duration
    // through Load; verify that the production container keeps it active and
    // preserves charge metadata for the next ability use.
    auto* restored = container.Load(RECAST_ABILITY, Recast::Sic, 20s, 10s, 3);

    bool ok = true;
    ok      = expectSeconds(restored->RecastTime, 20, "restored ability remaining duration") && ok;
    ok      = expectSeconds(restored->chargeTime, 10, "restored ability charge time") && ok;
    ok      = expectUInt(restored->maxCharges, 3, "restored ability max charges") && ok;
    ok      = expectBool(container.HasRecast(RECAST_ABILITY, Recast::Sic, 4s), true, "restored ability is active") && ok;
    return ok;
}

auto testDeletionAndResetSemantics() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    container.Load(RECAST_MAGIC, Recast::BloodPactRage, 30s);
    container.Load(RECAST_MAGIC, Recast::BloodPactWard, 45s);
    container.DeleteByIndex(RECAST_MAGIC, 0);

    container.Load(RECAST_ABILITY, Recast::Special, 3600s);
    container.Load(RECAST_ABILITY, Recast::Special2, 3600s);
    container.Load(RECAST_ABILITY, Recast::Sic, 30s);
    container.DeleteByIndex(RECAST_ABILITY, 2);

    bool ok = true;
    ok      = expectBool(container.Has(RECAST_MAGIC, Recast::BloodPactRage), false, "magic delete removes entry") && ok;
    ok      = expectBool(container.Has(RECAST_MAGIC, Recast::BloodPactWard), true, "magic delete keeps other entries") && ok;
    ok      = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Sic)->RecastTime, 0, "ability delete zeroes recast") && ok;

    container.Load(RECAST_ABILITY, Recast::Sic, 30s);
    container.ResetAbilities();
    ok = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Special)->RecastTime, 3600, "special survives reset") && ok;
    ok = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Special2)->RecastTime, 3600, "special2 survives reset") && ok;
    ok = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Sic)->RecastTime, 0, "normal ability reset") && ok;

    auto* charged = container.Load(RECAST_ABILITY, Recast::Sic, 20s, 10s, 3);
    container.ResetAbilities();
    ok = expectSeconds(charged->RecastTime, 20, "charged ability survives reset") && ok;
    return ok;
}

auto testDeleteAllSemantics() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    container.Load(RECAST_MAGIC, Recast::BloodPactRage, 30s);
    container.Load(RECAST_MAGIC, Recast::BloodPactWard, 45s);
    container.Load(RECAST_ABILITY, Recast::Sic, 30s);

    container.Del(RECAST_MAGIC);
    container.Del(RECAST_ABILITY);

    bool ok = true;
    ok      = expectUInt(container.GetRecastList(RECAST_MAGIC)->size(), 0, "magic Del clears list") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::Sic), true, "ability Del retains entry") && ok;
    ok      = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Sic)->RecastTime, 0, "ability Del zeroes recast") && ok;
    return ok;
}

auto testCheckExpiry() -> bool
{
    CBattleEntity     entity;
    CRecastContainer container(&entity);

    auto* magic   = container.Load(RECAST_MAGIC, Recast::BloodPactRage, 1s);
    auto* ability = container.Load(RECAST_ABILITY, Recast::Sic, 1s);
    magic->TimeStamp -= 2s;
    ability->TimeStamp -= 2s;

    container.Check();

    bool ok = true;
    ok      = expectBool(container.Has(RECAST_MAGIC, Recast::BloodPactRage), false, "expired magic removed") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::Sic), true, "expired ability retained") && ok;
    ok      = expectSeconds(container.GetRecast(RECAST_ABILITY, Recast::Sic)->RecastTime, 0, "expired ability zeroed") && ok;
    return ok;
}

auto testCharItemAndLootLists() -> bool
{
    CCharEntity          character;
    CCharRecastContainer container(&character);
    auto                 itemID = static_cast<Recast>(0x1204);

    auto* item = container.Load(RECAST_ITEM, itemID, 20s);
    container.AddLootRecast(LootRecastID::Seal, 30s);

    bool ok = true;
    ok      = expectBool(item != nullptr, true, "char item Load returns entry") && ok;
    ok      = expectUInt(container.GetRecastList(RECAST_ITEM)->size(), 1, "char item list size") && ok;
    ok      = expectBool(container.Has(RECAST_ITEM, itemID), true, "char has item recast") && ok;
    ok      = expectBool(container.HasLootRecast(LootRecastID::Seal), true, "char has loot recast") && ok;
    ok      = expectSeconds(container.GetLootRecast(LootRecastID::Seal)->RecastTime, 30, "char loot recast duration") && ok;

    container.DeleteByIndex(RECAST_ITEM, 0);
    container.Del(RECAST_LOOT);

    ok = expectUInt(container.GetRecastList(RECAST_ITEM)->size(), 0, "char item delete by index") && ok;
    ok = expectUInt(container.GetRecastList(RECAST_LOOT)->size(), 0, "char loot Del clears list") && ok;
    return ok;
}

auto testCharLootCheckExpiry() -> bool
{
    CCharEntity          character;
    CCharRecastContainer container(&character);

    auto* loot = container.Load(RECAST_LOOT, static_cast<Recast>(LootRecastID::Geode), 1s);
    loot->TimeStamp -= 2s;

    container.Check();

    return expectBool(container.HasLootRecast(LootRecastID::Geode), false, "expired char loot recast removed");
}

auto testCharChangeJob() -> bool
{
    CCharEntity          character;
    CCharRecastContainer container(&character);
    FakeDatabase         fake;

    container.Load(RECAST_ABILITY, Recast::Special, 3600s);
    container.Load(RECAST_ABILITY, Recast::Special2, 3600s);
    container.Load(RECAST_ABILITY, Recast::Sic, 30s);
    container.Load(RECAST_ABILITY, Recast::RandomDeal, 45s);

    db::setDatabase(&fake);
    container.ChangeJob();
    db::setDatabase(nullptr);

    bool ok = true;
    ok      = expectUInt(container.GetRecastList(RECAST_ABILITY)->size(), 2, "char ChangeJob ability list size") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::Special), true, "char ChangeJob keeps special") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::Special2), true, "char ChangeJob keeps special2") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::Sic), false, "char ChangeJob drops sic") && ok;
    ok      = expectBool(container.Has(RECAST_ABILITY, Recast::RandomDeal), false, "char ChangeJob drops random deal") && ok;
    return ok;
}

} // namespace

auto runRecastContainerSelfTests() -> bool
{
    bool ok = true;
    ok      = testLoadAndLookup() && ok;
    ok      = testChargedRecastAccumulation() && ok;
    ok      = testMutationBoundaries() && ok;
    ok      = testRestoredAbilityRecast() && ok;
    ok      = testDeletionAndResetSemantics() && ok;
    ok      = testDeleteAllSemantics() && ok;
    ok      = testCheckExpiry() && ok;
    ok      = testCharItemAndLootLists() && ok;
    ok      = testCharLootCheckExpiry() && ok;
    ok      = testCharChangeJob() && ok;
    return ok;
}
