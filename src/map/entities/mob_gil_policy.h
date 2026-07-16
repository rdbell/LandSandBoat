#pragma once

#include "common/cbasetypes.h"

namespace mobgilhelpers
{

// A negative maximum suppresses gil before the other modifiers are considered.
inline auto CanDropGil(const int16 gilMin, const int16 gilMax, const int16 gilBonus) -> bool
{
    return gilMax >= 0 && (gilMin > 0 || gilMax != 0 || gilBonus > 0);
}

} // namespace mobgilhelpers
