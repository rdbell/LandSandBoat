/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

namespace zoneentityvisibility
{

constexpr auto CharacterSyncPartySignificance = 100000.0f;
constexpr auto CharacterSyncAllianceSignificance = 10000.0f;

// CharacterSyncSignificance mirrors SpawnPCs scoring precedence: visible GMs
// and alliance peers have alliance significance; a same-party peer wins with
// the higher party significance.
constexpr auto CharacterSyncSignificance(
    const bool targetIsVisibleGM,
    const bool sameParty,
    const bool sameAlliance) -> float
{
    if (targetIsVisibleGM)
    {
        return CharacterSyncAllianceSignificance;
    }
    if (sameParty)
    {
        return CharacterSyncPartySignificance;
    }
    if (sameAlliance)
    {
        return CharacterSyncAllianceSignificance;
    }
    return 0.0f;
}

} // namespace zoneentityvisibility
