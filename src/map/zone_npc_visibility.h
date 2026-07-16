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

// ShouldSpawnNPC mirrors CZoneEntities::SpawnNPCs' combined NPC/transport
// predicate. Ship transports bypass status but never use always-relevant
// spawning; ordinary NPCs require a visible status and are range-or-global.
constexpr auto ShouldSpawnNPC(
    const bool isShip,
    const bool hasVisibleStatus,
    const bool inRange,
    const bool alwaysRelevant) -> bool
{
    if (isShip)
    {
        return !alwaysRelevant && inRange;
    }
    return hasVisibleStatus && (inRange || alwaysRelevant);
}

} // namespace zoneentityvisibility
