#pragma once

#include <chrono>
#include <cstdint>

namespace trustcontrollerreposition
{

inline constexpr auto Cooldown = std::chrono::seconds(3);

inline auto ShouldSelect(float currentDistance, float desiredDistance, bool currentPositionValid,
                         std::chrono::steady_clock::duration sinceLastReposition, bool inTransit) -> bool
{
    const bool outOfTolerance = currentDistance < desiredDistance - 2.5f || currentDistance > desiredDistance + 2.5f;
    return (outOfTolerance || !currentPositionValid) && sinceLastReposition > Cooldown && !inTransit;
}

inline auto ShouldPath(float distanceToPosition, uint8_t failedAttempts) -> bool
{
    return distanceToPosition > 2.0f && failedAttempts < 3;
}

} // namespace trustcontrollerreposition
