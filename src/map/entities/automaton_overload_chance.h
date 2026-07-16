#pragma once

#include "common/cbasetypes.h"

#include <algorithm>

namespace automatonoverloadhelpers
{

// Mirrors CAutomatonEntity::overloadChance's type-deduced integer clamp.
inline auto OverloadChance(const uint8 burden, const int16 overloadThresholdMod) -> uint8
{
    const int16 threshold = 30 + overloadThresholdMod;
    return std::clamp(static_cast<int>(burden) - threshold + 5, 0, 255);
}

} // namespace automatonoverloadhelpers
