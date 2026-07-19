#pragma once

#include <chrono>
#include <cstdint>

namespace mobcontrollerfollow
{
enum class Type : uint8_t { None, Roam, RunAway };

struct Result
{
    bool hasTarget;
    Type type;
    bool neutral;
    std::chrono::steady_clock::duration neutralAfter;
    bool notifyFollow;
    bool notifyUnfollow;
    bool clearOwnerAndEnmity;
};

constexpr auto SetTarget(
    const bool currentHasTarget, const Type currentType, const bool targetExists, const Type desiredType,
    const bool sameTarget, const bool healthFull, const std::chrono::steady_clock::duration tick) -> Result
{
    if (sameTarget && currentType == desiredType)
    {
        return { currentHasTarget, currentType, false, {}, false, false, false };
    }
    const bool clearingRoam = !targetExists && currentType == Type::Roam;
    return { targetExists, desiredType, clearingRoam, clearingRoam ? tick + std::chrono::seconds(30) : std::chrono::steady_clock::duration{},
             targetExists, clearingRoam, clearingRoam && healthFull };
}

// CanFollow reports whether a mob may begin following an aggro-eligible target.
constexpr auto CanFollow(
    const bool notNeutral, const bool hasFollowFlag, const bool hasNoFollowTarget,
    const bool hasNoFollowType, const bool canAggroTarget) -> bool
{
    return notNeutral && hasFollowFlag && hasNoFollowTarget && hasNoFollowType && canAggroTarget;
}

// ShouldClearOnEngage reports whether an existing roam follow target is cleared.
constexpr auto ShouldClearOnEngage(const bool hasFollowTarget, const bool isRoamFollow) -> bool
{
    return hasFollowTarget && isRoamFollow;
}
} // namespace mobcontrollerfollow
