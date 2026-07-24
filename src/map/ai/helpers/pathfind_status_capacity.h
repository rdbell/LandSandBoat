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

// ShouldTruncatePathPoints reports whether AddPoints must resize to max.
// Mirrors: points.size() > MAX_PATH_POINTS && !(PATHFLAG_PATROL)
// Formula (slice 6339): pointCount > maxPathPoints && !isPatrol
// Dual-wire of Go pathfind.ShouldTruncatePathPoints (add_points_limit.go).
// Call site: CPathFind::AddPoints before resize/ShowWarning.
inline auto ShouldTruncatePathPoints(const std::size_t pointCount, const std::size_t maxPathPoints, const bool isPatrol) -> bool
{
    return pointCount > maxPathPoints && !isPatrol;
}

// LimitDistanceReached reports whether moved distance hit LimitDistance.
// Mirrors: m_maxDistance > 0 && m_distanceMoved >= m_maxDistance
// Formula (slice 6339): maxDistance > 0 && distanceMoved >= maxDistance
// Dual-wire of Go pathfind.LimitDistanceReached (add_points_limit.go).
inline auto LimitDistanceReached(const float maxDistance, const float distanceMoved) -> bool
{
    return maxDistance > 0.f && distanceMoved >= maxDistance;
}

// ShouldPrunePath reports whether PrunePathWithin may enter the prune loop.
// Mirrors: if (!IsFollowingPath()) return;
// Formula (slice 6340): following
// Dual-wire of Go pathfind.ShouldPrunePath (prune_path.go).
inline auto ShouldPrunePath(const bool following) -> bool
{
    return following;
}

// ShouldContinuePrune reports whether to erase second-last for one more step.
// Mirrors: while (size > 1) { if (distance > within) break; erase; }
// Formula (slice 6340): pointCount > 1 && distanceToSecondLast <= within
// Dual-wire of Go pathfind.ShouldContinuePrune (prune_path.go).
// Host injects Euclidean distance(target, secondLast).
inline auto ShouldContinuePrune(const std::size_t pointCount, const float distanceToSecondLast, const float within) -> bool
{
    return pointCount > 1 && distanceToSecondLast <= within;
}

// ValidPosition reports the navmesh validPosition inject result.
// Mirrors: return navMesh()->validPosition(pos);
// Formula (slice 6340): meshValid
// Dual-wire of Go pathfind.ValidPositionResult (prune_path.go).
// Call site: CPathFind::ValidPosition.
inline auto ValidPosition(const bool meshValid) -> bool
{
    return meshValid;
}

// ShouldResumePatrol reports whether ResumePatrol may restore m_patrol.
// Mirrors: if (m_patrolFlags & PATHFLAG_PATROL) { ... }
// Formula (slice 6341): isPatrol
// Dual-wire of Go pathfind.ShouldResumePatrol (resume_patrol.go).
// Call site: CPathFind::ResumePatrol admission. Closest-point loop host-owned.
inline auto ShouldResumePatrol(const bool isPatrol) -> bool
{
    return isPatrol;
}

// ShouldUpdateLookAt reports whether LookAt should assign worldAngle rotation.
// Mirrors: if (!isWithinDistance(owner, point, 0.1f, true)) { set rotation }
// Formula (slice 6342): !withinFlat01
// Dual-wire of Go pathfind.ShouldUpdateLookAt (look_at.go).
// Call site: CPathFind::LookAt. Host injects isWithinDistance flat 0.1.
inline auto ShouldUpdateLookAt(const bool withinFlat01) -> bool
{
    return !withinFlat01;
}

// WaitStillActive reports whether FollowPath is still waiting at a waypoint.
// Mirrors: tick < m_timeAtPoint (still waiting) vs tick >= deadline (advance).
// Formula (slice 6342): now < deadline
// Dual-wire of Go pathfind.WaitStillActive (follow_wait_gates.go).
// Call site: CPathFind::FollowPath wait branch.
template <typename TimePoint>
inline auto WaitStillActive(const TimePoint now, const TimePoint deadline) -> bool
{
    return now < deadline;
}

// PathIndexComplete reports whether current path index is past the last point.
// Mirrors: m_currentPoint >= (int16)m_points.size()
// Formula (slice 6342): currentPoint >= pointCount
// Dual-wire of Go pathfind.PathIndexComplete (follow_wait_gates.go).
// Call sites: CPathFind::FollowPath wait-advance and end-of-path checks.
inline auto PathIndexComplete(const int currentPoint, const int pointCount) -> bool
{
    return currentPoint >= pointCount;
}

} // namespace pathfindstatushelpers
