#pragma once

#include <cstddef>

// Pure CPathFind follow/status injects (slice 6336).
// Dual-wire of Go pathfind.IsFollowingPathFlag /
// IsFollowingScriptedPathFlag / IsPatrollingFlag / OnPointFlag
// (internal/pathfind/follow_status.go).

namespace pathfindstatushelpers
{

// IsFollowingPath reports whether a non-empty point list is present.
// Mirrors: return !m_points.empty();
// Formula (slice 6336): pointCount > 0
inline auto IsFollowingPath(const std::size_t pointCount) -> bool
{
    return pointCount > 0;
}

// IsFollowingScriptedPath reports following with PATHFLAG_SCRIPT.
// Mirrors: return IsFollowingPath() && m_pathFlags & PATHFLAG_SCRIPT;
// Formula (slice 6336): following && scriptFlag
inline auto IsFollowingScriptedPath(const bool following, const bool scriptFlag) -> bool
{
    return following && scriptFlag;
}

// IsPatrolling reports PATHFLAG_PATROL on patrol flags.
// Mirrors: return m_patrolFlags & PATHFLAG_PATROL;
// Formula (slice 6336): patrolFlag
inline auto IsPatrolling(const bool patrolFlag) -> bool
{
    return patrolFlag;
}

// OnPoint reports the sticky on-point flag.
// Mirrors: return m_onPoint;
// Formula (slice 6336): onPoint
inline auto OnPoint(const bool onPoint) -> bool
{
    return onPoint;
}

// AtPointThreshold returns the isWithinDistance radius for AtPoint.
// Mirrors: m_distanceFromPoint == 0 ? 0.1f : m_distanceFromPoint + 0.2f
// Formula (slice 6337): distanceFromPoint == 0 ? 0.1 : distanceFromPoint + 0.2
// Dual-wire of Go pathfind.AtPointThreshold (at_point.go).
// Call site: CPathFind::AtPoint before host isWithinDistance.
inline auto AtPointThreshold(const float distanceFromPoint) -> float
{
    return distanceFromPoint == 0.f ? 0.1f : distanceFromPoint + 0.2f;
}

// IsWaterTerrain reports shallow or deep water membership for InWater.
// Mirrors: terrain == ShallowWater || terrain == DeepWater
// Formula (slice 6338): shallow || deep (host injects equality flags)
// Dual-wire of Go pathfind.IsWaterTerrain (in_water.go).
// Call site: CPathFind::InWater after GetTerrainAt.
inline auto IsWaterTerrain(const bool isShallowWater, const bool isDeepWater) -> bool
{
    return isShallowWater || isDeepWater;
}

} // namespace pathfindstatushelpers
