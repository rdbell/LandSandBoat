/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_spawn_slot.h"

#include "map/entities/mob_entity.h"
#include "map/spawn_handler.h"
#include "map/spawn_slot.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace
{

class TestMob final : public CMobEntity
{
public:
    explicit TestMob(const uint32 mobId, const bool alive = false)
    {
        id        = mobId;
        status    = alive ? STATUS_TYPE::NORMAL : STATUS_TYPE::DISAPPEAR;
        health.hp = alive ? 1 : 0;
    }

    void Spawn() override
    {
        ++spawnCount;
        allowedRespawnAtSpawn = m_AllowRespawn;
        status                = STATUS_TYPE::NORMAL;
        health.hp             = 1;
    }

    int  spawnCount{ 0 };
    bool allowedRespawnAtSpawn{ false };
};

auto expect(const bool condition, const std::string& label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn slot self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto selectionHooks(const uint32 roll, const std::size_t fallbackIndex = 0) -> SpawnSlotHooks
{
    SpawnSlotHooks hooks{};
    hooks.roll100      = [roll]()
    {
        return roll;
    };
    hooks.fallbackIndex = [fallbackIndex](const std::size_t)
    {
        return fallbackIndex;
    };
    hooks.canSpawnNow = [](const CMobEntity*)
    {
        return true;
    };
    return hooks;
}

auto expectSpawned(const TestMob& mob, const std::string& label) -> bool
{
    return expect(mob.spawnCount == 1, label + " spawn count") && expect(mob.allowedRespawnAtSpawn, label + " allow respawn before Spawn");
}

auto testAddRemoveAndEntries() -> bool
{
    bool      ok = true;
    SpawnSlot slot;
    TestMob   first(7);
    TestMob   sameId(7);

    slot.AddMob(&first, 10);
    slot.AddMob(&sameId, 20);
    slot.AddMob(&first, 30);

    const auto& entries = slot.GetEntries();
    ok                  = expect(entries.size() == 3, "AddMob preserves duplicates") && ok;
    ok                  = expect(entries[0].mob == &first && entries[1].mob == &sameId && entries[2].mob == &first, "AddMob append order") && ok;
    ok                  = expect(entries[0].spawnChance == 10 && entries[1].spawnChance == 20 && entries[2].spawnChance == 30, "AddMob chance order") && ok;
    ok                  = expect(first.GetSpawnSlot() == &slot && sameId.GetSpawnSlot() == &slot, "AddMob slot association") && ok;

    slot.RemoveMob(&first);
    ok = expect(slot.GetEntries().size() == 1 && slot.GetEntries()[0].mob == &sameId, "RemoveMob erases all pointer matches only") && ok;
    ok = expect(first.GetSpawnSlot() == &slot, "RemoveMob retains mob slot association") && ok;

    slot.RemoveMob(&first);
    ok = expect(slot.GetEntries().size() == 1, "RemoveMob absent pointer is no-op") && ok;
    slot.RemoveMob(&sameId);
    ok = expect(slot.IsEmpty(), "IsEmpty after removing last entry") && ok;

    return ok;
}

auto testEmptyIneligibleAndAlive() -> bool
{
    bool ok = true;

    {
        SpawnSlot slot;
        auto      hooks = selectionHooks(0);
        ok              = expect(!slot.TrySpawn(hooks), "empty TrySpawn returns false") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   mob(1);
        int       rollCalls     = 0;
        int       fallbackCalls = 0;
        slot.AddMob(&mob, 0);
        SpawnSlotHooks hooks{};
        hooks.canSpawnNow = [](const CMobEntity*)
        {
            return false;
        };
        hooks.roll100 = [&rollCalls]()
        {
            ++rollCalls;
            return 0;
        };
        hooks.fallbackIndex = [&fallbackCalls](const std::size_t)
        {
            ++fallbackCalls;
            return 0;
        };

        ok = expect(!slot.TrySpawn(hooks), "only-ineligible TrySpawn returns false") && ok;
        ok = expect(rollCalls == 0 && fallbackCalls == 0 && mob.spawnCount == 0, "ineligible path avoids randomness and Spawn") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   dead(1);
        TestMob   alive(2, true);
        TestMob   later(3);
        slot.AddMob(&dead, 100);
        slot.AddMob(&alive, 0);
        slot.AddMob(&later, 0);
        auto hooks = selectionHooks(0);

        ok = expect(!slot.TrySpawn(hooks), "alive member blocks slot") && ok;
        ok = expect(dead.spawnCount == 0 && later.spawnCount == 0, "alive member discards earlier candidate") && ok;
    }

    return ok;
}

auto testSpecificMobPath() -> bool
{
    bool ok = true;

    {
        SpawnSlot slot;
        TestMob   mob(10);
        int       conditionCalls = 0;
        slot.AddMob(&mob, 0);
        auto hooks        = selectionHooks(99);
        hooks.canSpawnNow = [&conditionCalls](const CMobEntity*)
        {
            ++conditionCalls;
            return true;
        };

        ok = expect(slot.TrySpawn(hooks, Maybe<uint32>{ 10 }), "specific dead eligible mob returns true") && ok;
        ok = expect(conditionCalls == 1, "specific dead mob checks condition") && ok;
        ok = expectSpawned(mob, "specific dead mob") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   mob(10, true);
        int       conditionCalls = 0;
        slot.AddMob(&mob, 0);
        auto hooks        = selectionHooks(0);
        hooks.canSpawnNow = [&conditionCalls](const CMobEntity*)
        {
            ++conditionCalls;
            return true;
        };

        ok = expect(!slot.TrySpawn(hooks, Maybe<uint32>{ 10 }), "specific alive mob returns false") && ok;
        ok = expect(conditionCalls == 0 && mob.spawnCount == 0, "specific alive mob skips condition and Spawn") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   mob(10);
        slot.AddMob(&mob, 0);
        auto hooks        = selectionHooks(0);
        hooks.canSpawnNow = [](const CMobEntity*)
        {
            return false;
        };

        ok = expect(!slot.TrySpawn(hooks, Maybe<uint32>{ 10 }), "specific condition rejection returns false") && ok;
        ok = expect(mob.spawnCount == 0, "specific condition rejection does not Spawn") && ok;
        ok = expect(!slot.TrySpawn(hooks, Maybe<uint32>{ 11 }), "specific missing ID returns false") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   first(10, true);
        TestMob   second(10);
        slot.AddMob(&first, 0);
        slot.AddMob(&second, 0);
        auto hooks = selectionHooks(0);

        ok = expect(!slot.TrySpawn(hooks, Maybe<uint32>{ 10 }), "specific ID uses alive first matching entry") && ok;
        ok = expect(second.spawnCount == 0, "specific ID does not continue to later duplicate-ID entry") && ok;
    }

    return ok;
}

auto testEligibilityOrder() -> bool
{
    bool                ok = true;
    SpawnSlot           slot;
    TestMob             first(1);
    TestMob             second(2);
    std::vector<uint32> checked;
    slot.AddMob(&first, 0);
    slot.AddMob(&second, 0);
    auto hooks        = selectionHooks(0);
    hooks.canSpawnNow = [&checked](const CMobEntity* mob)
    {
        checked.push_back(mob->id);
        return mob->id == 2;
    };

    ok = expect(slot.TrySpawn(hooks), "one eligible fallback returns true") && ok;
    ok = expect(checked == std::vector<uint32>{ 1, 2 }, "condition predicate entry order") && ok;
    ok = expect(first.spawnCount == 0, "ineligible mob not spawned") && ok;
    ok = expectSpawned(second, "eligible fallback") && ok;
    return ok;
}

auto testWeightedBoundaries() -> bool
{
    bool ok = true;
    for (const auto [roll, expectedId] : std::vector<std::pair<uint32, uint32>>{ { 0, 1 }, { 19, 1 }, { 20, 2 }, { 49, 2 } })
    {
        SpawnSlot slot;
        TestMob   first(1);
        TestMob   second(2);
        slot.AddMob(&first, 20);
        slot.AddMob(&second, 30);
        auto hooks = selectionHooks(roll);

        ok = expect(slot.TrySpawn(hooks), "weighted boundary returns true") && ok;
        ok = expect(first.spawnCount + second.spawnCount == 1, "weighted boundary spawns exactly one") && ok;
        ok = expect((expectedId == 1 ? first.spawnCount : second.spawnCount) == 1, "weighted cumulative boundary selects expected mob") && ok;
        ok = expect((expectedId == 1 ? first.allowedRespawnAtSpawn : second.allowedRespawnAtSpawn), "weighted selection allows respawn before Spawn") && ok;
    }
    return ok;
}

auto testChanceTotalsAndFallbacks() -> bool
{
    bool ok = true;

    {
        SpawnSlot slot;
        TestMob   mob(1);
        slot.AddMob(&mob, 50);
        auto hooks = selectionHooks(50);
        ok         = expect(!slot.TrySpawn(hooks), "total below 100 failed roll without fallback returns false") && ok;
        ok         = expect(mob.spawnCount == 0, "failed roll without fallback does not Spawn") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   first(1);
        TestMob   second(2);
        slot.AddMob(&first, 40);
        slot.AddMob(&second, 60);
        auto hooks = selectionHooks(99);
        ok         = expect(slot.TrySpawn(hooks), "total equal 100 accepts roll 99") && ok;
        ok         = expectSpawned(second, "total equal 100") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   first(1);
        TestMob   second(2);
        slot.AddMob(&first, 80);
        slot.AddMob(&second, 80);
        auto hooks = selectionHooks(99);
        ok         = expect(slot.TrySpawn(hooks), "total above 100 accepts roll 99") && ok;
        ok         = expectSpawned(second, "total above 100 cumulative selection") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   chance(1);
        TestMob   firstFallback(2);
        TestMob   secondFallback(3);
        int       fallbackSize = 0;
        slot.AddMob(&chance, 50);
        slot.AddMob(&firstFallback, 0);
        slot.AddMob(&secondFallback, 0);
        auto hooks           = selectionHooks(75, 1);
        hooks.fallbackIndex = [&fallbackSize](const std::size_t size)
        {
            fallbackSize = static_cast<int>(size);
            return 1;
        };

        ok = expect(slot.TrySpawn(hooks), "failed chance roll uses fallback") && ok;
        ok = expect(fallbackSize == 2, "fallback sees only zero-chance candidates") && ok;
        ok = expect(chance.spawnCount == 0 && firstFallback.spawnCount == 0, "unselected chance and fallback mobs do not Spawn") && ok;
        ok = expectSpawned(secondFallback, "selected fallback") && ok;
    }

    {
        SpawnSlot slot;
        TestMob   mob(1);
        int       rollCalls = 0;
        slot.AddMob(&mob, 0);
        auto hooks      = selectionHooks(0);
        hooks.roll100 = [&rollCalls]()
        {
            ++rollCalls;
            return 0;
        };

        ok = expect(slot.TrySpawn(hooks), "zero-chance-only fallback returns true") && ok;
        ok = expect(rollCalls == 0, "zero total chance skips roll") && ok;
        ok = expectSpawned(mob, "zero-chance fallback") && ok;
    }

    return ok;
}

auto testRespawnRegistry() -> bool
{
    using namespace std::chrono_literals;

    bool         ok = true;
    SpawnHandler handler(nullptr);

    ok = expect(!handler.isRegistered(nullptr), "null mob is not registered") && ok;
    ok = expect(!handler.getRemainingRespawnTime(nullptr).has_value(), "null mob has no remaining respawn") && ok;
    handler.registerForRespawn(nullptr);
    handler.unregister(nullptr);

    TestMob disabled(20);
    disabled.m_AllowRespawn = false;
    disabled.m_RespawnTime  = 30min;
    handler.registerForRespawn(&disabled);
    ok = expect(!handler.isRegistered(&disabled), "respawn-disabled mob is not registered") && ok;

    TestMob unslotted(21);
    unslotted.m_AllowRespawn = true;
    unslotted.m_RespawnTime  = 30min;
    handler.registerForRespawn(&unslotted);
    ok = expect(handler.isRegistered(&unslotted), "unslotted mob is registered by ID") && ok;
    const auto unslottedRemaining = handler.getRemainingRespawnTime(&unslotted);
    ok = expect(unslottedRemaining.has_value() && *unslottedRemaining > 29min && *unslottedRemaining <= 30min,
                "default respawn delay supplies unslotted deadline") && ok;
    handler.unregister(&unslotted);
    ok = expect(!handler.isRegistered(&unslotted) && !handler.getRemainingRespawnTime(&unslotted).has_value(),
                "unregister removes unslotted deadline") && ok;

    SpawnSlot slot;
    TestMob   first(30);
    TestMob   second(31);
    first.m_AllowRespawn  = true;
    second.m_AllowRespawn = true;
    first.m_RespawnTime   = 20min;
    second.m_RespawnTime  = 25min;
    slot.AddMob(&first, 0);
    slot.AddMob(&second, 0);

    handler.registerForRespawn(&first);
    ok = expect(handler.isRegistered(&first) && handler.isRegistered(&second), "slot members share one registration") && ok;
    handler.registerForRespawn(&second, 40min);
    const auto replacedRemaining = handler.getRemainingRespawnTime(&first);
    ok = expect(replacedRemaining.has_value() && *replacedRemaining > 39min && *replacedRemaining <= 40min,
                "slot registration is replaced by override") && ok;
    handler.unregister(&first);
    ok = expect(!handler.isRegistered(&second), "unregistering one slot member clears shared registration") && ok;

    handler.registerForRespawn(&unslotted, -1s);
    const auto overdueRemaining = handler.getRemainingRespawnTime(&unslotted);
    ok = expect(overdueRemaining.has_value() && *overdueRemaining == timer::duration::zero(),
                "overdue registration remains present and clamps remaining time to zero") && ok;

    return ok;
}

} // namespace

auto runSpawnSlotSelfTests() -> bool
{
    bool ok = true;
    ok      = testAddRemoveAndEntries() && ok;
    ok      = testEmptyIneligibleAndAlive() && ok;
    ok      = testSpecificMobPath() && ok;
    ok      = testEligibilityOrder() && ok;
    ok      = testWeightedBoundaries() && ok;
    ok      = testChanceTotalsAndFallbacks() && ok;
    ok      = testRespawnRegistry() && ok;
    return ok;
}
