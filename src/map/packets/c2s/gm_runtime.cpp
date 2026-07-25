/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "gm_runtime.h"

#include <algorithm>

#include "common/logging.h"

auto gmhelpers::CommandFrom(const uint8_t (&command)[115], const uint16_t headerSize) -> std::string
{
    const auto messageLength = std::min<std::size_t>((headerSize * 4U) - 0x4U, sizeof(command));
    return asStringFromUntrustedSource(command, messageLength);
}
