#pragma once

#include <cmath>
#include <cstdint>

// Pure DrawIn decision halves (mesh/nav/packets remain host-side).

namespace drawinhelpers
{

constexpr float EntityHeight    = 2.0f;
constexpr float PostSnapYDelta  = 1.0f;

inline auto DegreesToRadians(const float degrees) -> float
{
    return degrees * (static_cast<float>(M_PI) / 180.0f);
}

constexpr auto ShouldAbortDrawInNoZone(const bool hasZone) -> bool
{
    return !hasZone;
}

constexpr auto ShouldAbortDrawInRayBlock(const bool rayIntersects) -> bool
{
    return rayIntersects;
}

constexpr auto DrawInRaySourceY(const float posY) -> float
{
    return posY - EntityHeight;
}

constexpr auto DrawInAfterSnapY(const float nearY) -> float
{
    return nearY - PostSnapYDelta;
}

constexpr auto ShouldApplyDrawInMove(const bool isCutsceneOnly, const bool isDead, const bool isMounted) -> bool
{
    if (isCutsceneOnly)
    {
        return false;
    }
    return !isDead && !isMounted;
}

} // namespace drawinhelpers
