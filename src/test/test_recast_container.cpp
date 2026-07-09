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
    ok      = testDeletionAndResetSemantics() && ok;
    ok      = testDeleteAllSemantics() && ok;
    ok      = testCheckExpiry() && ok;
    ok      = testCharItemAndLootLists() && ok;
    ok      = testCharLootCheckExpiry() && ok;
    ok      = testCharChangeJob() && ok;
    return ok;
}
