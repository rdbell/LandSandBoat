/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_zone_npc_visibility_2697.h"

#include "map/zone_npc_visibility.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone NPC visibility 2697 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runZoneNPCVisibility2697SelfTests() -> bool
{
    using zoneentityvisibility::ShouldSpawnNPC;

    bool ok = true;
    ok      = expect(ShouldSpawnNPC(false, true, true, false), "normal in-range NPC spawns") && ok;
    ok      = expect(ShouldSpawnNPC(false, true, false, true), "always-relevant NPC bypasses range") && ok;
    ok      = expect(!ShouldSpawnNPC(false, false, true, true), "hidden always-relevant NPC does not spawn") && ok;
    ok      = expect(!ShouldSpawnNPC(false, true, false, false), "ordinary out-of-range NPC does not spawn") && ok;
    ok      = expect(ShouldSpawnNPC(true, false, true, false), "in-range ship bypasses status") && ok;
    ok      = expect(!ShouldSpawnNPC(true, true, true, true), "always-relevant ship is excluded") && ok;
    ok      = expect(!ShouldSpawnNPC(true, true, false, false), "out-of-range ship does not spawn") && ok;
    return ok;
}
