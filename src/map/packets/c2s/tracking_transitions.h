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

#include <cstdint>

#include "common/types/maybe.h"

namespace tracking
{

// TargetIdentity is the entity data retained while a character wide-scans a
// target. It intentionally has no CBaseEntity dependency so packet behavior
// can be tested without a map entity.
struct TargetIdentity
{
    uint32_t id{};
    uint16_t targid{};

    auto operator==(const TargetIdentity&) const -> bool = default;
};

// WideScanRangeFor preserves the range selected by the character capability
// calculation for a TRACKING_LIST request.
auto WideScanRangeFor(uint16_t range) -> uint16_t;

// StartTargetFor selects the retained wide-scan target. A lookup miss clears
// the target; an unscannable or out-of-range target leaves it untouched.
auto StartTargetFor(Maybe<TargetIdentity> currentTarget, Maybe<TargetIdentity> requestedTarget, bool isWideScannable, bool isWithinRange) -> Maybe<TargetIdentity>;

// EndTargetFor clears the retained wide-scan target for TRACKING_END.
auto EndTargetFor() -> Maybe<TargetIdentity>;

} // namespace tracking
