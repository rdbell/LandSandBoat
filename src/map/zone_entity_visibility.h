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

#include <cmath>

namespace zoneentityvisibility
{

constexpr auto VerticalRenderDistance       = 20.0f;
constexpr auto VerticalRenderDistanceOffset = 0.5f;

// IsWithinVerticalRenderDistance mirrors zone-entity spawning's asymmetric
// vertical visibility check. NPCs deliberately bypass this policy.
inline auto IsWithinVerticalRenderDistance(const float sourceY, const float targetY) -> bool
{
    return std::abs(targetY - sourceY - VerticalRenderDistanceOffset) <= VerticalRenderDistance;
}

} // namespace zoneentityvisibility
