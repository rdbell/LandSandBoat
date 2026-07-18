#pragma once

#include "common/mmo.h"
#include "common/utils.h"

namespace targetfindradiushelpers
{
// IsWithinRadius mirrors CTargetFind's inclusive three-dimensional radius checks.
inline auto IsWithinRadius(const position_t& origin, const position_t& candidate, const float radius) -> bool
{
    return distance(origin, candidate) <= radius;
}
} // namespace targetfindradiushelpers
