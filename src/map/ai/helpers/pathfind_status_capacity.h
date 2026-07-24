#pragma once

#include <cstddef>
#include <cstdint>

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

// ShouldSnapCareful reports whether FollowPath should snap to valid position.
// Mirrors: if (m_carefulPathing) { snapToValidPosition }
// Formula (slice 6343): carefulPathing
// Dual-wire of Go pathfind.ShouldSnapCareful (follow_admissions.go).
inline auto ShouldSnapCareful(const bool carefulPathing) -> bool
{
    return carefulPathing;
}

// ShouldStartWaypointWait reports whether FollowPath starts a point wait.
// Mirrors: wait != 0s && m_timeAtPoint == min
// Formula (slice 6343): waitNonzero && !hasActiveWait
// Dual-wire of Go pathfind.ShouldStartWaypointWait (follow_admissions.go).
inline auto ShouldStartWaypointWait(const bool waitNonzero, const bool hasActiveWait) -> bool
{
    return waitNonzero && !hasActiveWait;
}

// CarefulPathingValue returns the careful flag for SetCarefulPathing.
// Mirrors: m_carefulPathing = careful
// Formula (slice 6343): careful
// Dual-wire of Go pathfind.CarefulPathingValue (follow_admissions.go).
inline auto CarefulPathingValue(const bool careful) -> bool
{
    return careful;
}

// ShouldBlockNonScriptPath reports whether PathTo rejects a non-script path
// while a scripted path is already active.
// Mirrors: following && (m_pathFlags & SCRIPT) && !(pathFlags & SCRIPT)
// Formula (slice 6344): following && currentScript && !newScript
// Dual-wire of Go pathfind.ShouldBlockNonScriptPath (path_to_gates.go).
// Call site: CPathFind::PathTo admission.
inline auto ShouldBlockNonScriptPath(const bool following, const bool currentScript, const bool newScript) -> bool
{
    return following && currentScript && !newScript;
}

// ShouldUseWallhackPath reports whether PathTo uses FindClosestPath.
// Mirrors: m_pathFlags & PATHFLAG_WALLHACK
// Formula (slice 6344): wallhack
// Dual-wire of Go pathfind.ShouldUseWallhackPath (path_to_gates.go).
inline auto ShouldUseWallhackPath(const bool wallhack) -> bool
{
    return wallhack;
}

// StepDistance returns StepTo walk/run step length.
// Mirrors: speed / (run ? 50 : 40)
// Formula (slice 6344): run ? speed/50 : speed/40
// Dual-wire of Go pathfind.StepDistance (path_to_gates.go).
// Call site: CPathFind::StepTo.
inline auto StepDistance(const float speed, const bool run) -> float
{
    return speed / (run ? 50.f : 40.f);
}

// ShouldSnapToTarget reports whether StepTo is within one step of the target.
// Mirrors: distanceTo <= m_distanceFromPoint + stepDistance
// Formula (slice 6344): distanceTo <= rangeFromTarget + stepDistance
// Dual-wire of Go pathfind.ShouldSnapToTarget (path_to_gates.go).
inline auto ShouldSnapToTarget(const float distanceTo, const float rangeFromTarget, const float stepDistance) -> bool
{
    return distanceTo <= rangeFromTarget + stepDistance;
}

// ShouldReversePoints reports whether AddPoints reverses the point list.
// Mirrors: AddPoints(points, m_pathFlags & PATHFLAG_REVERSE)
// Formula (slice 6345): reverseFlag
// Dual-wire of Go pathfind.ShouldReversePoints (path_flags_gates.go).
inline auto ShouldReversePoints(const bool reverseFlag) -> bool
{
    return reverseFlag;
}

// ShouldClearBeforePath reports whether PathTo clears for the clear argument.
// Mirrors: if (clear) Clear();
// Formula (slice 6345): clear
// Dual-wire of Go pathfind.ShouldClearBeforePath (path_flags_gates.go).
inline auto ShouldClearBeforePath(const bool clear) -> bool
{
    return clear;
}

// WormStepSpeedOverride reports worm roam speed when entity speed is zero.
// Mirrors: if (speed==0 && ROAMFLAG_WORM) speed = 20;
// Formula (slice 6345): speedZero && wormRoam → use 20
// Dual-wire of Go pathfind.WormStepSpeedOverride (path_flags_gates.go).
// Call site: CPathFind::StepTo. outSpeed is only valid when return is true.
inline auto WormStepSpeedOverride(const bool speedZero, const bool wormRoam, float& outSpeed) -> bool
{
    if (speedZero && wormRoam)
    {
        outSpeed = 20.f;
        return true;
    }
    return false;
}

// MovingDelta returns the StepTo movement-counter increment.
// Mirrors: speedChange ? 0x28 : 0x35
// Formula (slice 6345): speedChanged ? 0x28 : 0x35
// Dual-wire of Go pathfind.MovingDelta (path_flags_gates.go).
inline auto MovingDelta(const bool speedChanged) -> uint16_t
{
    return speedChanged ? 0x28 : 0x35;
}

// WrapMoving applies StepTo moving counter update with 0x2000 modulus.
// Mirrors: moving += delta; moving %= 0x2000;
// Formula (slice 6345): (moving + MovingDelta(speedChanged)) % 0x2000
// Dual-wire of Go pathfind.WrapMoving (path_flags_gates.go).
inline auto WrapMoving(const uint16_t moving, const bool speedChanged) -> uint16_t
{
    return static_cast<uint16_t>((moving + MovingDelta(speedChanged)) % 0x2000);
}

// ShouldFollowPath reports whether FollowPath may run this tick.
// Mirrors: if (!IsFollowingPath()) return;
// Formula (slice 6346): following
// Dual-wire of Go pathfind.ShouldFollowPath (follow_entry.go).
// Call site: CPathFind::FollowPath entry.
inline auto ShouldFollowPath(const bool following) -> bool
{
    return following;
}

// ShouldStepVertical reports whether StepTo uses vertical slope stepping.
// Mirrors: if (abs(diff_y) > .5f) { slope } else { y = target.y }
// Formula (slice 6346): absDiffY > 0.5
// Dual-wire of Go pathfind.ShouldStepVertical (follow_entry.go).
// Call site: CPathFind::StepTo vertical branches.
inline auto ShouldStepVertical(const float absDiffY) -> bool
{
    return absDiffY > 0.5f;
}

// HasDestination reports whether a destination point is present.
// Mirrors non-empty m_points precondition for GetDestination pure half.
// Formula (slice 6346): pointCount > 0
// Dual-wire of Go pathfind.HasDestination (follow_entry.go).
inline auto HasDestination(const std::size_t pointCount) -> bool
{
    return pointCount > 0;
}

// ClampVerticalStep clamps new_y between start and end vertical positions.
// Mirrors min_y/max_y clamp after slope step in StepTo.
// Formula (slice 6346): clamp(newY, min(startY,endY), max(startY,endY))
// Dual-wire of Go pathfind.ClampVerticalStep (follow_entry.go).
inline auto ClampVerticalStep(const float newY, const float startY, const float endY) -> float
{
    const float minY = startY < endY ? startY : endY;
    const float maxY = startY > endY ? startY : endY;
    if (newY < minY)
    {
        return minY;
    }
    if (newY > maxY)
    {
        return maxY;
    }
    return newY;
}

// DistanceFromPointValue returns range stored for PathInRange/PathAround.
// Mirrors: m_distanceFromPoint = range;
// Formula (slice 6347): range
// Dual-wire of Go pathfind.DistanceFromPointValue (warp_path_in_range.go).
inline auto DistanceFromPointValue(const float range) -> float
{
    return range;
}

// LimitDistanceValue returns max length stored by LimitDistance.
// Mirrors: m_maxDistance = maxLength;
// Formula (slice 6347): maxLength
// Dual-wire of Go pathfind.LimitDistanceValue (warp_path_in_range.go).
inline auto LimitDistanceValue(const float maxLength) -> float
{
    return maxLength;
}

// WarpMovingReset returns movement counter after WarpTo.
// Mirrors: m_POwner->loc.p.moving = 0;
// Formula (slice 6347): 0
// Dual-wire of Go pathfind.WarpMovingReset (warp_path_in_range.go).
inline auto WarpMovingReset() -> uint16_t
{
    return 0;
}

// ShouldNotifyZoneOnMove reports whether WarpTo notifies the zone.
// Mirrors: if (m_POwner->loc.zone != nullptr) onEntityMoved
// Formula (slice 6347): hasZone
// Dual-wire of Go pathfind.ShouldNotifyZoneOnMove (warp_path_in_range.go).
// Call site: CPathFind::WarpTo. Callback remains host-owned.
inline auto ShouldNotifyZoneOnMove(const bool hasZone) -> bool
{
    return hasZone;
}

// ClearedOnPoint returns m_onPoint after Clear.
// Formula (slice 6349): true
// Dual-wire of Go pathfind.ClearedOnPoint (clear_defaults.go).
inline auto ClearedOnPoint() -> bool
{
    return true;
}

// ClearedCurrentPoint returns m_currentPoint after Clear.
// Formula (slice 6349): 0
// Dual-wire of Go pathfind.ClearedCurrentPoint (clear_defaults.go).
inline auto ClearedCurrentPoint() -> int16_t
{
    return 0;
}

// ClearedDistanceFromPoint returns m_distanceFromPoint after Clear.
// Formula (slice 6349): 0
inline auto ClearedDistanceFromPoint() -> float
{
    return 0.f;
}

// ClearedMaxDistance returns m_maxDistance after Clear.
// Formula (slice 6349): 0
inline auto ClearedMaxDistance() -> float
{
    return 0.f;
}

// ClearedDistanceMoved returns m_distanceMoved after Clear.
// Formula (slice 6349): 0
inline auto ClearedDistanceMoved() -> float
{
    return 0.f;
}

// ClearedCurrentTurn returns m_currentTurn after Clear.
// Formula (slice 6349): 0
inline auto ClearedCurrentTurn() -> int
{
    return 0;
}

// ClearedPathFlags returns m_pathFlags after Clear.
// Formula (slice 6349): 0
inline auto ClearedPathFlags() -> uint8_t
{
    return 0;
}

// ClearedRoamFlags returns m_roamFlags after Clear.
// Formula (slice 6349): 0
inline auto ClearedRoamFlags() -> uint16_t
{
    return 0;
}

// ArePositionsClose reports whether FindPath/FindClosestPath reject the query.
// Mirrors free arePositionsClose: distance(a,b) < 1.0f
// Formula (slice 6348): distance < 1.0
// Dual-wire of Go pathfind.ArePositionsClose (find_path_gates.go).
// Host injects distance(start, end). Call sites: FindPath, FindClosestPath.
inline auto ArePositionsClose(const float distance) -> bool
{
    return distance < 1.0f;
}

// FindPathSucceeded reports whether FindPath may return true after mesh query.
// Mirrors: !m_points.empty()
// Formula (slice 6348): pointCount > 0
// Dual-wire of Go pathfind.FindPathSucceeded (find_path_gates.go).
inline auto FindPathSucceeded(const std::size_t pointCount) -> bool
{
    return pointCount > 0;
}

// RandomPathPolyRadius returns FindRandomPath poly-query radius.
// Mirrors: maxRadius / 10.0f
// Formula (slice 6348): maxRadius / 10
// Dual-wire of Go pathfind.RandomPathPolyRadius (find_path_gates.go).
inline auto RandomPathPolyRadius(const float maxRadius) -> float
{
    return maxRadius / 10.0f;
}

// ShouldClearAfterFailedPath reports Clear after failed FindPath-style result.
// Mirrors: if (!result) Clear();
// Formula (slice 6348): !result
// Dual-wire of Go pathfind.ShouldClearAfterFailedPath (find_path_gates.go).
// Call sites: PathTo, FinishedPath NextTurn fail, RoamAround fail.
inline auto ShouldClearAfterFailedPath(const bool result) -> bool
{
    return !result;
}

// RandomPathHasTurns reports whether FindRandomPath can path to first turn.
// Mirrors: if (m_turnPoints.size() > 0)
// Formula (slice 6348): turnCount > 0
// Dual-wire of Go pathfind.RandomPathHasTurns (find_path_gates.go).
inline auto RandomPathHasTurns(const std::size_t turnCount) -> bool
{
    return turnCount > 0;
}

// HasActiveWaypointWait reports whether FollowPath is in the wait-deadline branch.
// Mirrors: m_timeAtPoint != timer::time_point::min()
// Formula (slice 6350): hasDeadline
// Dual-wire of Go pathfind.HasActiveWaypointWait (follow_point_gates.go).
// Call sites: CPathFind::FollowPath wait branch entry; ShouldStartWaypointWait second arg.
inline auto HasActiveWaypointWait(const bool hasDeadline) -> bool
{
    return hasDeadline;
}

// ShouldApplyPointRotation reports whether FollowPath applies baked point rotation.
// Mirrors: if (targetPoint.setRotation)
// Formula (slice 6350): setRotation
// Dual-wire of Go pathfind.ShouldApplyPointRotation (follow_point_gates.go).
inline auto ShouldApplyPointRotation(const bool setRotation) -> bool
{
    return setRotation;
}

// ShouldStepWithRun reports whether StepTo is invoked with run=true from FollowPath.
// Mirrors: StepTo(targetPoint.position, m_pathFlags & PATHFLAG_RUN)
// Formula (slice 6350): runFlag
// Dual-wire of Go pathfind.ShouldStepWithRun (follow_point_gates.go).
inline auto ShouldStepWithRun(const bool runFlag) -> bool
{
    return runFlag;
}

// RoamFlagsValue returns the roam flags stored by RoamAround.
// Mirrors: m_roamFlags = roamFlags
// Formula (slice 6350): roamFlags
// Dual-wire of Go pathfind.RoamFlagsValue (follow_point_gates.go).
// Call site: CPathFind::RoamAround after Clear.
inline auto RoamFlagsValue(const uint16_t roamFlags) -> uint16_t
{
    return roamFlags;
}

// CompletedOnPoint returns m_onPoint after path completion in FollowPath.
// Mirrors: m_onPoint = true after FinishedPath
// Formula (slice 6350): true
// Dual-wire of Go pathfind.CompletedOnPoint (follow_point_gates.go).
inline auto CompletedOnPoint() -> bool
{
    return true;
}

// ArrivedOnPoint returns m_onPoint when AtPoint succeeds in FollowPath.
// Mirrors: m_onPoint = true when AtPoint(targetPoint.position)
// Formula (slice 6350): true
// Dual-wire of Go pathfind.ArrivedOnPoint (follow_point_gates.go).
inline auto ArrivedOnPoint() -> bool
{
    return true;
}

// LeavingPoint returns m_onPoint when FollowPath begins moving after wait branch.
// Mirrors: m_onPoint = false at start of move half
// Formula (slice 6350): false
// Dual-wire of Go pathfind.LeavingPoint (follow_point_gates.go).
inline auto LeavingPoint() -> bool
{
    return false;
}

// PathFlagsValue returns the path flags stored by PathTo / PathThrough.
// Mirrors: m_pathFlags = pathFlags
// Formula (slice 6351): pathFlags
// Dual-wire of Go pathfind.PathFlagsValue (patrol_install_gates.go).
inline auto PathFlagsValue(const uint8_t pathFlags) -> uint8_t
{
    return pathFlags;
}

// ShouldSnapshotPatrol reports whether AddPoints copies into m_patrol.
// Mirrors: if (m_pathFlags & PATHFLAG_PATROL)
// Formula (slice 6351): isPatrol
// Dual-wire of Go pathfind.ShouldSnapshotPatrol (patrol_install_gates.go).
inline auto ShouldSnapshotPatrol(const bool isPatrol) -> bool
{
    return isPatrol;
}

// PatrolFlagsValue returns patrol flags stored when snapshotting a patrol.
// Mirrors: m_patrolFlags = m_pathFlags
// Formula (slice 6351): pathFlags
// Dual-wire of Go pathfind.PatrolFlagsValue (patrol_install_gates.go).
inline auto PatrolFlagsValue(const uint8_t pathFlags) -> uint8_t
{
    return pathFlags;
}

// ClearedPatrolFlags returns m_patrolFlags for the non-patrol AddPoints branch.
// Mirrors: m_patrolFlags = 0
// Formula (slice 6351): 0
// Dual-wire of Go pathfind.ClearedPatrolFlags (patrol_install_gates.go).
inline auto ClearedPatrolFlags() -> uint8_t
{
    return 0;
}

// RestartedCurrentPoint returns m_currentPoint after RestartPatrol.
// Mirrors FinishedPath: m_currentPoint = 0
// Formula (slice 6351): 0
// Dual-wire of Go pathfind.RestartedCurrentPoint (patrol_install_gates.go).
inline auto RestartedCurrentPoint() -> int16_t
{
    return 0;
}

// RestartedCurrentTurn returns m_currentTurn after RestartPatrol.
// Mirrors FinishedPath: m_currentTurn = 0
// Formula (slice 6351): 0
// Dual-wire of Go pathfind.RestartedCurrentTurn (patrol_install_gates.go).
inline auto RestartedCurrentTurn() -> int
{
    return 0;
}

// ShouldSnapExact reports whether StepTo snap branch copies target position.
// Mirrors: if (m_distanceFromPoint == 0)
// Formula (slice 6352): distanceFromPoint == 0
// Dual-wire of Go pathfind.ShouldSnapExact (step_snap_gates.go).
// Call site: CPathFind::StepTo after ShouldSnapToTarget.
inline auto ShouldSnapExact(const float distanceFromPoint) -> bool
{
    return distanceFromPoint == 0.f;
}

// SnapDistanceMoved returns distance accounted when snapping to target.
// Mirrors: m_distanceMoved += distanceTo - m_distanceFromPoint
// Formula (slice 6352): distanceTo - distanceFromPoint
// Dual-wire of Go pathfind.SnapDistanceMoved (step_snap_gates.go).
inline auto SnapDistanceMoved(const float distanceTo, const float distanceFromPoint) -> float
{
    return distanceTo - distanceFromPoint;
}

// FindPathCursor returns m_currentPoint after a mesh FindPath-style install.
// Mirrors FindPath / FindClosestPath / FindRandomPath: m_currentPoint = 0
// Formula (slice 6352): 0
// Dual-wire of Go pathfind.FindPathCursor (step_snap_gates.go).
inline auto FindPathCursor() -> int16_t
{
    return 0;
}

// ResumePatrolCursor returns m_currentPoint before closest-point scan.
// Mirrors ResumePatrol: m_currentPoint = 0
// Formula (slice 6353): 0
// Dual-wire of Go pathfind.ResumePatrolCursor (resume_patrol_gates.go).
inline auto ResumePatrolCursor() -> int16_t
{
    return 0;
}

// InitialClosestPatrolDistance returns the seed for closest-point search.
// Mirrors: float closestPoint = FLT_MAX
// Formula (slice 6353): FLT_MAX
// Dual-wire of Go pathfind.InitialClosestPatrolDistance (resume_patrol_gates.go).
inline auto InitialClosestPatrolDistance() -> float
{
    return 3.402823466e+38f; // FLT_MAX
}

// ShouldUpdateClosestPatrol reports whether a candidate is nearer than closest.
// Mirrors: if (distanceSq < closestPoint)
// Formula (slice 6353): distance < closestSoFar
// Dual-wire of Go pathfind.ShouldUpdateClosestPatrol (resume_patrol_gates.go).
// Host injects distanceSquared (C++) or Euclidean distance (Go); order matches.
inline auto ShouldUpdateClosestPatrol(const float distance, const float closestSoFar) -> bool
{
    return distance < closestSoFar;
}

// OriginalPointX returns PathAround slide-target X.
// Mirrors: m_originalPoint.x = point.x
// Formula (slice 6354): x
// Dual-wire of Go pathfind.OriginalPointX (path_around_clear_gates.go).
inline auto OriginalPointX(const float x) -> float
{
    return x;
}

// OriginalPointY returns PathAround slide-target Y.
// Formula (slice 6354): y
inline auto OriginalPointY(const float y) -> float
{
    return y;
}

// OriginalPointZ returns PathAround slide-target Z.
// Formula (slice 6354): z
inline auto OriginalPointZ(const float z) -> float
{
    return z;
}

// ClearedWaiting reports whether a wait deadline is active after Clear.
// Mirrors: m_timeAtPoint = timer::time_point::min() (no active wait)
// Formula (slice 6354): false
// Dual-wire of Go pathfind.ClearedWaiting (path_around_clear_gates.go).
// Call site: CPathFind::Clear wait reset (semantic pure half; host assigns min()).
inline auto ClearedWaiting() -> bool
{
    return false;
}

// ClearedWaitDeadline documents inactive wait tick after Clear (Go half).
// Formula (slice 6354): 0
// Dual-wire of Go pathfind.ClearedWaitDeadline. C++ host uses time_point::min().
inline auto ClearedWaitDeadline() -> int64_t
{
    return 0;
}

// OriginalPointMoving returns PathAround original slide-target moving counter.
// Mirrors: m_originalPoint.moving = point.moving
// Formula (slice 6355): moving
// Dual-wire of Go pathfind.OriginalPointMoving (prune_wait_advance_gates.go).
inline auto OriginalPointMoving(const uint16_t moving) -> uint16_t
{
    return moving;
}

// OriginalPointRotation returns PathAround original slide-target rotation.
// Mirrors: m_originalPoint.rotation = point.rotation
// Formula (slice 6355): rotation
// Dual-wire of Go pathfind.OriginalPointRotation (prune_wait_advance_gates.go).
inline auto OriginalPointRotation(const uint8_t rotation) -> uint8_t
{
    return rotation;
}

// ShouldPruneHasPair reports whether PrunePathWithin may access second-last.
// Mirrors: if (m_points.size() < 2) break
// Formula (slice 6355): pointCount >= 2
// Dual-wire of Go pathfind.ShouldPruneHasPair (prune_wait_advance_gates.go).
// Call site: CPathFind::PrunePathWithin before second-last access.
inline auto ShouldPruneHasPair(const std::size_t pointCount) -> bool
{
    return pointCount >= 2;
}

// AdvancedCurrentPoint returns path cursor after one waypoint advance.
// Mirrors: ++m_currentPoint
// Formula (slice 6355): current + 1
// Dual-wire of Go pathfind.AdvancedCurrentPoint (prune_wait_advance_gates.go).
// Call site: CPathFind::FollowPath wait-expired branch.
inline auto AdvancedCurrentPoint(const int16_t current) -> int16_t
{
    return static_cast<int16_t>(current + 1);
}

// ShouldIteratePathPoint reports whether FollowPath arrival loop may continue.
// Mirrors: while (m_currentPoint < (int16)m_points.size())
// Formula (slice 6356): current < pointCount
// Dual-wire of Go pathfind.ShouldIteratePathPoint (follow_iterate_gates.go).
// Call site: CPathFind::FollowPath arrival while condition.
inline auto ShouldIteratePathPoint(const int current, const int pointCount) -> bool
{
    return current < pointCount;
}

// AdvancedCurrentTurn returns m_currentTurn after FinishedPath increments it.
// Mirrors: m_currentTurn++
// Formula (slice 6356): current + 1
// Dual-wire of Go pathfind.AdvancedCurrentTurn (follow_iterate_gates.go).
// Call site: CPathFind::FinishedPath entry.
inline auto AdvancedCurrentTurn(const int current) -> int
{
    return current + 1;
}

// WaitDeadlineFrom returns wait deadline as now + wait (monotonic tick units).
// Mirrors: m_timeAtPoint = tick + targetPoint.wait
// Formula (slice 6356): now + wait
// Dual-wire of Go pathfind.WaitDeadlineFrom (follow_iterate_gates.go).
// Production C++ chrono time_point + duration remains host-composed; pure half
// pins arithmetic for dual-wire self-tests and the Go production path.
inline auto WaitDeadlineFrom(const int64_t now, const int64_t wait) -> int64_t
{
    return now + wait;
}

// HasNextRoamTurn reports whether FinishedPath should path to the next roam turn.
// Mirrors after m_currentTurn++: m_currentTurn < m_turnPoints.size()
// Formula (slice 6357): currentTurn < turnCount
// Dual-wire of Go pathfind.HasNextRoamTurn (finish_path_next_turn.go).
// Call site: CPathFind::FinishedPath Resolve first argument.
inline auto HasNextRoamTurn(const int currentTurn, const std::size_t turnCount) -> bool
{
    return currentTurn < static_cast<int>(turnCount);
}

// LookAtWithinDistance returns the flat radius for LookAt admission.
// Mirrors: isWithinDistance(owner, point, 0.1f, true)
// Formula (slice 6358): 0.1
// Dual-wire of Go pathfind.LookAtWithinDistance (look_at.go).
// Call site: CPathFind::LookAt before ShouldUpdateLookAt.
inline auto LookAtWithinDistance() -> float
{
    return 0.1f;
}

} // namespace pathfindstatushelpers
