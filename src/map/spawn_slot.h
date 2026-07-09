/*
===========================================================================
  Copyright (c) 2021 Eden Dev Teams
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/
===========================================================================
*/

#pragma once

#include <common/cbasetypes.h>
#include <common/types/maybe.h>

#include <functional>

class CMobEntity;

struct SpawnSlotEntry
{
    CMobEntity* mob;

    // Chance out of 100 of this mob spawning out of the mobs sharing the slot.
    // If not all mobs in the slot have a chance defined, then the ones without it
    // will be rolled between equally, if none of the ones with a specified chance succeeds.
    uint8 spawnChance{ 0 };
};

// Optional per-call dependencies for deterministic slot selection. Empty
// functions retain the live SpawnHandler and xirand behavior.
struct SpawnSlotHooks
{
    std::function<uint32()>                  roll100;
    std::function<std::size_t(std::size_t)> fallbackIndex;
    std::function<bool(const CMobEntity*)>  canSpawnNow;
};

class SpawnSlot
{
public:
    void AddMob(CMobEntity* mob, uint8 spawnChance);
    void RemoveMob(const CMobEntity* mob);
    auto TrySpawn(Maybe<uint32> specificMobId = std::nullopt) -> bool;
    auto TrySpawn(const SpawnSlotHooks& hooks, Maybe<uint32> specificMobId = std::nullopt) -> bool;
    auto IsEmpty() const -> bool;
    auto GetEntries() const -> const std::vector<SpawnSlotEntry>&;

private:
    auto trySpawn(Maybe<uint32> specificMobId, const SpawnSlotHooks* hooks) -> bool;

    std::vector<SpawnSlotEntry> entries;
};
