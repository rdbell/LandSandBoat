/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "tracking_transitions.h"

auto tracking::WideScanRangeFor(uint16_t range) -> uint16_t
{
    return range;
}

auto tracking::StartTargetFor(Maybe<TargetIdentity> currentTarget, Maybe<TargetIdentity> requestedTarget, bool isWideScannable, bool isWithinRange) -> Maybe<TargetIdentity>
{
    if (!requestedTarget)
    {
        return std::nullopt;
    }

    if (!isWideScannable || !isWithinRange)
    {
        return currentTarget;
    }

    return requestedTarget;
}

auto tracking::EndTargetFor() -> Maybe<TargetIdentity>
{
    return std::nullopt;
}
