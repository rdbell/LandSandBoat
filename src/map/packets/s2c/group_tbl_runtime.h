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

#include "common/cbasetypes.h"

namespace grouptblhelpers
{

constexpr auto MaxEntries = 20U;

[[nodiscard]] inline auto HasCapacity(const uint8 entryCount) -> bool
{
    return entryCount < MaxEntries;
}

} // namespace grouptblhelpers
