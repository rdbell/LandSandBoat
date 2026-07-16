/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <functional>
#include <string>
#include <vector>

#include "common/cbasetypes.h"

namespace autotranslate
{

using Lookup = std::function<std::string(const std::string&, const std::vector<uint16>&)>;

// Replaces autotranslate byte blocks using lookup for each complete block.
std::string replaceBytesWithLookup(const std::string& str, const Lookup& lookup);

} // namespace autotranslate
