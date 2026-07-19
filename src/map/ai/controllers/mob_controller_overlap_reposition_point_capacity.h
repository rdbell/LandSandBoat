#pragma once

#include "common/mmo.h"
#include "common/utils.h"

#include <cmath>

namespace mobcontrolleroverlaprepositionpoint
{
// Destination returns the point used to separate overlapping spawned mobs.
inline auto Destination(const position_t& mobPosition, const position_t& targetPosition) -> position_t
{
    const auto angle = static_cast<uint8>(worldAngle(mobPosition, targetPosition) + 64);

    return {
        mobPosition.x - (cosf(rotationToRadian(angle)) * 1.5f),
        targetPosition.y,
        mobPosition.z + (sinf(rotationToRadian(angle)) * 1.5f),
        0,
        0,
    };
}
} // namespace mobcontrolleroverlaprepositionpoint
