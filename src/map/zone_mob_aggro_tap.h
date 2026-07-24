#pragma once

namespace zoneaggrohelpers
{

// ShouldSkipMobAggro mirrors tapMobAggro's early exclusions.
constexpr auto ShouldSkipMobAggro(const bool mobDead, const bool charDead, const bool visibleGM, const bool hasMaster) -> bool
{
    return mobDead || charDead || visibleGM || hasMaster;
}

// ShouldUseRoamFollow mirrors the ROAMFLAG_FOLLOW branch selection.
constexpr auto ShouldUseRoamFollow(const bool hasFollowFlag) -> bool
{
    return hasFollowFlag;
}

// ShouldAttemptMobAggro mirrors tapMobAggro's eligibility gate before it calls
// CMobController::CanAggroTarget.
constexpr auto ShouldAttemptMobAggro(const bool difficultyAboveTooWeak, const bool charSitting, const bool alwaysAggro) -> bool
{
    return difficultyAboveTooWeak || charSitting || alwaysAggro;
}

} // namespace zoneaggrohelpers
