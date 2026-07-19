#pragma once

#include <chrono>
#include <cstdint>

namespace trustcontrollerreposition
{

enum class CompletionAction
{
    Path,
    Face,
};

struct CompletionPlan
{
    CompletionAction action;
    bool             clearTransit;
};

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

inline auto ResolveCompletion(float distanceToPosition, uint8_t failedAttempts) -> CompletionPlan
{
    if (ShouldPath(distanceToPosition, failedAttempts))
    {
        return { CompletionAction::Path, false };
    }
    return { CompletionAction::Face, true };
}

constexpr auto NextFailedAttempts(const bool hasTopEnmity, const uint8_t previous) -> uint8_t
{
    return hasTopEnmity ? static_cast<uint8_t>(previous + 1) : 0;
}

} // namespace trustcontrollerreposition
