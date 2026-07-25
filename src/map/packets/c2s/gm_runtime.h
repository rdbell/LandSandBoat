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
#include <string>

namespace gmhelpers
{

// CommandFrom extracts the variable-length GM command using the packet's
// advertised size. It never reads past the 115-byte command storage.
auto CommandFrom(const uint8_t (&command)[115], uint16_t headerSize) -> std::string;

} // namespace gmhelpers
