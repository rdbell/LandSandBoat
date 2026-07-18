#pragma once

#include "common/mmo.h"

namespace targetfindconehelpers
{
// IsWithinTriangle mirrors CTargetFind::isWithinCone's inclusive X/Z barycentric check.
inline auto IsWithinTriangle(const position_t& origin, const position_t& b, const position_t& c, const float scalar, const position_t& candidate) -> bool
{
    const float pointX = candidate.x - origin.x;
    const float pointZ = candidate.z - origin.z;
    const float weightX = (pointX * (b.z - c.z) + pointZ * (c.x - b.x) + b.x * c.z - c.x * b.z) / scalar;
    const float weightY = (pointX * c.z - pointZ * c.x) / scalar;
    const float weightZ = (pointZ * b.x - pointX * b.z) / scalar;
    return weightX >= 0 && weightX <= 1 && weightY >= 0 && weightY <= 1 && weightZ >= 0 && weightZ <= 1;
}
} // namespace targetfindconehelpers
