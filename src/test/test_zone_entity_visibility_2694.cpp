/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_zone_entity_visibility_2694.h"

#include "map/zone_entity_visibility.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone entity visibility 2694 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runZoneEntityVisibility2694SelfTests() -> bool
{
    using zoneentityvisibility::IsWithinVerticalRenderDistance;

    bool ok = true;
    ok      = expect(IsWithinVerticalRenderDistance(0.0f, 0.0f), "same elevation is visible") && ok;
    ok      = expect(IsWithinVerticalRenderDistance(0.0f, 20.5f), "upper inclusive edge includes offset") && ok;
    ok      = expect(!IsWithinVerticalRenderDistance(0.0f, 20.5001f), "upper side outside edge is hidden") && ok;
    ok      = expect(IsWithinVerticalRenderDistance(0.0f, -19.5f), "lower inclusive edge includes offset") && ok;
    ok      = expect(!IsWithinVerticalRenderDistance(0.0f, -19.5001f), "lower side outside edge is hidden") && ok;
    return ok;
}
