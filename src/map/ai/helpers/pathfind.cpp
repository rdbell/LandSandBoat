/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "pathfind.h"

#include "pathfind_finished_capacity.h"
#include "pathfind_status_capacity.h"

#include "ai/ai_container.h"

#include "common/utils.h"

#include "entities/base_entity.h"
#include "entities/mob_entity.h"

#include "lua/luautils.h"

#include "map/navmesh/navmesh.h"
#include "mob_modifier.h"
#include "status_effect_container.h"
#include "zone.h"

// arePositionsClose dual-wired as pathfindstatushelpers::ArePositionsClose (slice 6348).

CPathFind::CPathFind(CBaseEntity* PTarget)
: m_POwner(PTarget)
, m_distanceFromPoint(0.0f)
, m_pathFlags(0)
, m_patrolFlags(0)
, m_roamFlags(0)
, m_onPoint(false)
, m_currentPoint(0)
, m_currentTurn(0)
, m_distanceMoved(0.0f)
, m_maxDistance(0.0f)
, m_carefulPathing(false)
{
    m_originalPoint.x        = 0.0f;
    m_originalPoint.y        = 0.0f;
    m_originalPoint.z        = 0.0f;
    m_originalPoint.moving   = 0;
    m_originalPoint.rotation = 0;

    Clear();
}

CPathFind::~CPathFind()
{
    m_POwner = nullptr;
    Clear();
}

bool CPathFind::RoamAround(const position_t& point, float maxRadius, uint8 maxTurns, uint16 roamFlags)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    Clear();

    // Dual-wire: pathfindstatushelpers::RoamFlagsValue (slice 6350).
    m_roamFlags = pathfindstatushelpers::RoamFlagsValue(roamFlags);

    bool result = FindRandomPath(point, maxRadius, maxTurns, roamFlags);
    // Dual-wire: pathfindstatushelpers::ShouldClearAfterFailedPath (slice 6348).
    if (pathfindstatushelpers::ShouldClearAfterFailedPath(result))
    {
        Clear();
        return false;
    }

    return true;
}

bool CPathFind::PathTo(const position_t& point, uint8 pathFlags, bool clear)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ShouldBlockNonScriptPath (slice 6344).
    // don't follow a new path if the current path has script flag and new path doesn't
    if (pathfindstatushelpers::ShouldBlockNonScriptPath(
            IsFollowingPath(),
            (m_pathFlags & PATHFLAG_SCRIPT) != 0,
            (pathFlags & PATHFLAG_SCRIPT) != 0))
    {
        return false;
    }

    // Dual-wire: pathfindstatushelpers::ShouldClearBeforePath (slice 6345).
    if (pathfindstatushelpers::ShouldClearBeforePath(clear))
    {
        Clear();
    }

    // Dual-wire: pathfindstatushelpers::PathFlagsValue (slice 6351).
    m_pathFlags = pathfindstatushelpers::PathFlagsValue(pathFlags);

    bool result = false;

    // Dual-wire: pathfindstatushelpers::ShouldUseWallhackPath (slice 6344).
    if (pathfindstatushelpers::ShouldUseWallhackPath((m_pathFlags & PATHFLAG_WALLHACK) != 0))
    {
        result = FindClosestPath(m_POwner->loc.p, point);
    }
    else
    {
        result = FindPath(m_POwner->loc.p, point);
    }

    // Dual-wire: pathfindstatushelpers::ShouldClearAfterFailedPath (slice 6348).
    if (pathfindstatushelpers::ShouldClearAfterFailedPath(result))
    {
        Clear();
    }

    return result;
}

bool CPathFind::PathInRange(const position_t& point, float range, uint8 pathFlags /*= 0*/, bool clear /*= true*/)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: ShouldClearBeforePath (6345) + DistanceFromPointValue (6347).
    if (pathfindstatushelpers::ShouldClearBeforePath(clear))
    {
        Clear();
    }

    m_distanceFromPoint = pathfindstatushelpers::DistanceFromPointValue(range);

    bool result = PathTo(point, pathFlags, false);

    PrunePathWithin(range);
    return result;
}

bool CPathFind::PathAround(const position_t& point, float distanceFromPoint, uint8 pathFlags)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: ShouldClearBeforePath(true) (slice 6345).
    if (pathfindstatushelpers::ShouldClearBeforePath(true))
    {
        Clear();
    }

    // Dual-wire: OriginalPoint* (slice 6354) — save for sliding logic.
    m_originalPoint.x        = pathfindstatushelpers::OriginalPointX(point.x);
    m_originalPoint.y        = pathfindstatushelpers::OriginalPointY(point.y);
    m_originalPoint.z        = pathfindstatushelpers::OriginalPointZ(point.z);
    m_originalPoint.moving   = point.moving;
    m_originalPoint.rotation = point.rotation;
    // Dual-wire: DistanceFromPointValue (slice 6347).
    m_distanceFromPoint = pathfindstatushelpers::DistanceFromPointValue(distanceFromPoint);

    // Don't clear path so
    // original point / distance are kept
    return PathTo(point, pathFlags, false);
}

bool CPathFind::PathThrough(std::vector<pathpoint_t>&& points, uint8 pathFlags)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    Clear();

    // Dual-wire: pathfindstatushelpers::PathFlagsValue (slice 6351).
    m_pathFlags = pathfindstatushelpers::PathFlagsValue(pathFlags);

    // Dual-wire: pathfindstatushelpers::ShouldReversePoints (slice 6345).
    AddPoints(std::move(points), pathfindstatushelpers::ShouldReversePoints((m_pathFlags & PATHFLAG_REVERSE) != 0));

    return true;
}

bool CPathFind::WarpTo(const position_t& point, float maxDistance)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    Clear();

    position_t newPoint = nearPosition(point, maxDistance, (float)M_PI);

    m_POwner->loc.p.x      = newPoint.x;
    m_POwner->loc.p.y      = newPoint.y;
    m_POwner->loc.p.z      = newPoint.z;
    // Dual-wire: pathfindstatushelpers::WarpMovingReset (slice 6347).
    m_POwner->loc.p.moving = pathfindstatushelpers::WarpMovingReset();

    LookAt(point);
    m_POwner->updatemask |= UPDATE_POS;

    // Dual-wire: pathfindstatushelpers::ShouldNotifyZoneOnMove (slice 6347).
    if (pathfindstatushelpers::ShouldNotifyZoneOnMove(m_POwner->loc.zone != nullptr))
    {
        m_POwner->loc.zone->onEntityMoved(m_POwner);
    }

    return true;
}

void CPathFind::ResumePatrol()
{
    // Dual-wire: pathfindstatushelpers::ShouldResumePatrol (slice 6341).
    if (pathfindstatushelpers::ShouldResumePatrol((m_patrolFlags & PATHFLAG_PATROL) != 0))
    {
        // Dual-wire: PathFlagsValue (6351) + ResumePatrol closest injects (6353).
        m_pathFlags    = pathfindstatushelpers::PathFlagsValue(m_patrolFlags);
        m_points       = m_patrol;
        m_currentPoint = pathfindstatushelpers::ResumePatrolCursor();
        float closestPoint = pathfindstatushelpers::InitialClosestPatrolDistance();
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const float distanceSq = distanceSquared(m_POwner->loc.p, m_points[i].position);
            if (pathfindstatushelpers::ShouldUpdateClosestPatrol(distanceSq, closestPoint))
            {
                m_currentPoint = (int16)i;
                closestPoint   = distanceSq;
            }
        }
    }
}

bool CPathFind::ValidPosition(const position_t& pos)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ValidPosition (slice 6340).
    return pathfindstatushelpers::ValidPosition(m_POwner->loc.zone->navMesh()->validPosition(pos));
}

void CPathFind::LimitDistance(float maxLength)
{
    // Dual-wire: pathfindstatushelpers::LimitDistanceValue (slice 6347).
    m_maxDistance = pathfindstatushelpers::LimitDistanceValue(maxLength);
}

void CPathFind::PrunePathWithin(float within)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ShouldPrunePath (slice 6340).
    if (!pathfindstatushelpers::ShouldPrunePath(IsFollowingPath()))
    {
        return;
    }

    position_t targetPoint = m_points.back().position;

    while (true)
    {
        if (m_points.size() < 2)
        {
            break;
        }
        position_t secondLastPoint = m_points[m_points.size() - 2].position;
        // Dual-wire: pathfindstatushelpers::ShouldContinuePrune (slice 6340).
        if (!pathfindstatushelpers::ShouldContinuePrune(m_points.size(), distance(targetPoint, secondLastPoint), within))
        {
            break;
        }
        m_points.erase(m_points.end() - 2);
    }
}

void CPathFind::FollowPath(timer::time_point tick)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ShouldFollowPath (slice 6346).
    if (!pathfindstatushelpers::ShouldFollowPath(IsFollowingPath()))
    {
        return;
    }

    // Dual-wire: pathfindstatushelpers::HasActiveWaypointWait (slice 6350).
    if (pathfindstatushelpers::HasActiveWaypointWait(m_timeAtPoint != timer::time_point::min()))
    {
        // Dual-wire: WaitStillActive / PathIndexComplete (slice 6342).
        // Continue to wait until full wait time has elapsed
        if (!pathfindstatushelpers::WaitStillActive(tick, m_timeAtPoint))
        {
            m_timeAtPoint = timer::time_point::min();
            ++m_currentPoint;
            luautils::OnPathPoint(m_POwner);
            if (pathfindstatushelpers::PathIndexComplete(m_currentPoint, static_cast<int>(m_points.size())))
            {
                luautils::OnPathComplete(m_POwner);
                FinishedPath();
            }
        }
        return;
    }

    // Dual-wire: pathfindstatushelpers::LeavingPoint (slice 6350).
    m_onPoint = pathfindstatushelpers::LeavingPoint();

    pathpoint_t targetPoint = m_points[m_currentPoint];

    // Dual-wire: pathfindstatushelpers::ShouldSnapCareful (slice 6343).
    if (pathfindstatushelpers::ShouldSnapCareful(m_carefulPathing))
    {
        m_POwner->loc.zone->navMesh()->snapToValidPosition(m_POwner->loc.p);
    }

    // Dual-wire: pathfindstatushelpers::LimitDistanceReached (slice 6339).
    if (pathfindstatushelpers::LimitDistanceReached(m_maxDistance, m_distanceMoved))
    {
        // if I have a max distance, check to stop me
        // Clear sets onPoint via ClearedOnPoint (slice 6349).
        Clear();
        return;
    }

    // Iterate over points in the current path and find the first point
    // that we haven't successfully arrived at already.
    while (m_currentPoint < (int16)m_points.size())
    {
        targetPoint = m_points[m_currentPoint];

        if (AtPoint(targetPoint.position))
        {
            // Dual-wire: pathfindstatushelpers::ArrivedOnPoint (slice 6350).
            m_onPoint = pathfindstatushelpers::ArrivedOnPoint();
            // Dual-wire: pathfindstatushelpers::ShouldApplyPointRotation (slice 6350).
            if (pathfindstatushelpers::ShouldApplyPointRotation(targetPoint.setRotation))
            {
                m_POwner->loc.p.rotation = targetPoint.position.rotation;
                m_POwner->updatemask |= UPDATE_POS;
            }
            // Dual-wire: ShouldStartWaypointWait (6343) + HasActiveWaypointWait (6350).
            if (pathfindstatushelpers::ShouldStartWaypointWait(
                    targetPoint.wait != 0s,
                    pathfindstatushelpers::HasActiveWaypointWait(m_timeAtPoint != timer::time_point::min())))
            {
                m_timeAtPoint = tick + targetPoint.wait;
                return;
            }

            luautils::OnPathPoint(m_POwner);
            m_currentPoint++;
        }
        else
        {
            break;
        }
    }

    // Dual-wire: pathfindstatushelpers::ShouldStepWithRun (slice 6350).
    StepTo(targetPoint.position, pathfindstatushelpers::ShouldStepWithRun((m_pathFlags & PATHFLAG_RUN) != 0));

    // Dual-wire: pathfindstatushelpers::PathIndexComplete (slice 6342).
    if (pathfindstatushelpers::PathIndexComplete(m_currentPoint, static_cast<int>(m_points.size())))
    {
        luautils::OnPathComplete(m_POwner);
        FinishedPath();
        // Dual-wire: pathfindstatushelpers::CompletedOnPoint (slice 6350).
        m_onPoint = pathfindstatushelpers::CompletedOnPoint();
    }
}

void CPathFind::StepTo(const position_t& pos, bool run)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    bool  speedChange = m_POwner->GetSpeed() != m_POwner->UpdateSpeed(run);
    float speed       = m_POwner->GetSpeed();

    if (const auto* PMobEntity = dynamic_cast<CMobEntity*>(m_POwner))
    {
        // Dual-wire: pathfindstatushelpers::WormStepSpeedOverride (slice 6345).
        float wormSpeed = 0.f;
        if (pathfindstatushelpers::WormStepSpeedOverride(
                PMobEntity->GetSpeed() == 0,
                (m_roamFlags & ROAMFLAG_WORM) != 0,
                wormSpeed))
        {
            speed = wormSpeed;
        }
    }

    // Dual-wire: pathfindstatushelpers::StepDistance / ShouldSnapToTarget (slice 6344).
    float stepDistance = pathfindstatushelpers::StepDistance(speed, run);
    float distanceTo   = distance(m_POwner->loc.p, pos);
    float diff_y       = pos.y - m_POwner->loc.p.y;

    // face point mob is moving towards
    LookAt(pos);

    if (pathfindstatushelpers::ShouldSnapToTarget(distanceTo, m_distanceFromPoint, stepDistance))
    {
        // Dual-wire: SnapDistanceMoved / ShouldSnapExact (slice 6352).
        m_distanceMoved += pathfindstatushelpers::SnapDistanceMoved(distanceTo, m_distanceFromPoint);

        if (pathfindstatushelpers::ShouldSnapExact(m_distanceFromPoint))
        {
            m_POwner->loc.p.x = pos.x;
            m_POwner->loc.p.y = pos.y;
            m_POwner->loc.p.z = pos.z;
        }
        else
        {
            float radians = (1 - (float)m_POwner->loc.p.rotation / 256) * 2 * (float)M_PI;

            m_POwner->loc.p.x += cosf(radians) * (distanceTo - m_distanceFromPoint);
            m_POwner->loc.p.z += sinf(radians) * (distanceTo - m_distanceFromPoint);
            // Dual-wire: ShouldStepVertical / ClampVerticalStep (slice 6346).
            if (pathfindstatushelpers::ShouldStepVertical(abs(diff_y)))
            {
                // Don't step too far vertically by simply utilizing the slope
                float new_y = m_POwner->loc.p.y + stepDistance * (pos.y - m_POwner->loc.p.y) / distance(m_POwner->loc.p, pos, true);
                m_POwner->loc.p.y = pathfindstatushelpers::ClampVerticalStep(new_y, m_POwner->loc.p.y, pos.y);
            }
            else
            {
                m_POwner->loc.p.y = pos.y;
            }
        }
    }
    else
    {
        m_distanceMoved += stepDistance;
        // take a step towards target point
        float radians = (1 - (float)m_POwner->loc.p.rotation / 256) * 2 * (float)M_PI;

        m_POwner->loc.p.x += cosf(radians) * stepDistance;
        m_POwner->loc.p.z += sinf(radians) * stepDistance;
        // Dual-wire: ShouldStepVertical / ClampVerticalStep (slice 6346).
        if (pathfindstatushelpers::ShouldStepVertical(abs(diff_y)))
        {
            // Don't step too far vertically by simply utilizing the slope
            float new_y = m_POwner->loc.p.y + stepDistance * (pos.y - m_POwner->loc.p.y) / distance(m_POwner->loc.p, pos, true);
            m_POwner->loc.p.y = pathfindstatushelpers::ClampVerticalStep(new_y, m_POwner->loc.p.y, pos.y);
        }
        else
        {
            m_POwner->loc.p.y = pos.y;
        }
    }

    // Dual-wire: pathfindstatushelpers::WrapMoving (slice 6345).
    m_POwner->loc.p.moving = pathfindstatushelpers::WrapMoving(m_POwner->loc.p.moving, speedChange);

    m_POwner->updatemask |= UPDATE_POS;

    // Dual-wire: pathfindstatushelpers::ShouldNotifyZoneOnMove (slice 6347 / StepTo 6354).
    if (pathfindstatushelpers::ShouldNotifyZoneOnMove(m_POwner->loc.zone != nullptr))
    {
        m_POwner->loc.zone->onEntityMoved(m_POwner);
    }
}

bool CPathFind::FindPath(const position_t& start, const position_t& end)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ArePositionsClose (slice 6348).
    if (pathfindstatushelpers::ArePositionsClose(distance(start, end)))
    {
        return false;
    }

    m_points = m_POwner->loc.zone->navMesh()->findPath(start, end);
    // Dual-wire: pathfindstatushelpers::FindPathCursor (slice 6352).
    m_currentPoint = pathfindstatushelpers::FindPathCursor();

    // Dual-wire: pathfindstatushelpers::FindPathSucceeded (slice 6348).
    if (!pathfindstatushelpers::FindPathSucceeded(m_points.size()))
    {
        DebugNavmesh("CPathFind::FindPath Entity (%s - %d) could not find path", m_POwner->getName(), m_POwner->id);
        return false;
    }

    return true;
}

bool CPathFind::FindRandomPath(const position_t& start, float maxRadius, uint8 maxTurns, uint16 roamFlags)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    auto m_turnLength = static_cast<uint8_t>(xirand::GetRandomNumber<uint32>(maxTurns) + 1);

    // Dual-wire: pathfindstatushelpers::RandomPathPolyRadius (slice 6348).
    // Seemingly arbitrary value to pass for maxRadius, all values seem to give similar results, likely due to navmesh polygons being too dense?
    float      maxRadiusForPolyQuery = pathfindstatushelpers::RandomPathPolyRadius(maxRadius);
    position_t startPosition         = start;

    // find end points for turns, iterate potentially twice as many times to account for erroneous turnPoints
    for (int i = 0; i < m_turnLength * 2; i++)
    {
        // look for new turnPoint. findRandomPosition doesn't guarantee the new point is within the radius
        auto status = m_POwner->loc.zone->navMesh()->findRandomPosition(startPosition, maxRadiusForPolyQuery);

        // couldn't find one point so just break out
        if (status.first != 0)
        {
            return false;
        }

        // only add the roam point if it's _actually_ within range of the spawn point...
        if (isWithinDistance(startPosition, status.second, maxRadius, true))
        {
            m_turnPoints.emplace_back(status.second);
        }
        // else
        // {
        //     ShowDebug("CPathFind::FindRandomPath (%s - %d) random point too far: sq distance (%f)", m_POwner->GetName(), m_POwner->id, distSq);
        // }

        if (m_turnPoints.size() >= m_turnLength)
        {
            break;
        }
    }
    // Dual-wire: pathfindstatushelpers::RandomPathHasTurns (slice 6348).
    if (pathfindstatushelpers::RandomPathHasTurns(m_turnPoints.size()))
    {
        m_points = m_POwner->loc.zone->navMesh()->findPath(start, m_turnPoints[0]);
        // Dual-wire: pathfindstatushelpers::FindPathCursor (slice 6352).
        m_currentPoint = pathfindstatushelpers::FindPathCursor();
    }

    // Dual-wire: pathfindstatushelpers::FindPathSucceeded (slice 6348).
    return pathfindstatushelpers::FindPathSucceeded(m_points.size());
}

bool CPathFind::FindClosestPath(const position_t& start, const position_t& end)
{
    TracyZoneScoped;
    TracyZoneString(m_POwner->getName());

    // Dual-wire: pathfindstatushelpers::ArePositionsClose (slice 6348).
    if (pathfindstatushelpers::ArePositionsClose(distance(start, end)))
    {
        return false;
    }

    m_points = m_POwner->loc.zone->navMesh()->findPath(start, end);
    // Dual-wire: pathfindstatushelpers::FindPathCursor (slice 6352).
    m_currentPoint = pathfindstatushelpers::FindPathCursor();
    m_points.emplace_back(pathpoint_t{ end, 0s, false }); // this prevents exploits with navmesh / impassible terrain

    /* this check requirement is never met as intended since m_points are never empty when mob has a path
    if (m_points.empty())
    {
        // this is a trick to make mobs go up / down impassible terrain
        m_points.emplace_back(end);
    }
*/

    return true;
}

void CPathFind::LookAt(const position_t& point)
{
    // Dual-wire: pathfindstatushelpers::ShouldUpdateLookAt (slice 6342).
    // Avoid unpredictable results if we're too close.
    if (pathfindstatushelpers::ShouldUpdateLookAt(isWithinDistance(m_POwner->loc.p, point, 0.1f, true)))
    {
        m_POwner->loc.p.rotation = worldAngle(m_POwner->loc.p, point);
        m_POwner->updatemask |= UPDATE_POS;
    }
}

bool CPathFind::OnPoint() const
{
    // Dual-wire: pathfindstatushelpers::OnPoint (slice 6336).
    return pathfindstatushelpers::OnPoint(m_onPoint);
}

bool CPathFind::IsFollowingPath()
{
    // Dual-wire: pathfindstatushelpers::IsFollowingPath (slice 6336).
    return pathfindstatushelpers::IsFollowingPath(m_points.size());
}

bool CPathFind::IsFollowingScriptedPath()
{
    // Dual-wire: pathfindstatushelpers::IsFollowingScriptedPath (slice 6336).
    return pathfindstatushelpers::IsFollowingScriptedPath(
        IsFollowingPath(),
        (m_pathFlags & PATHFLAG_SCRIPT) != 0);
}

bool CPathFind::IsPatrolling()
{
    // Dual-wire: pathfindstatushelpers::IsPatrolling (slice 6336).
    return pathfindstatushelpers::IsPatrolling((m_patrolFlags & PATHFLAG_PATROL) != 0);
}

bool CPathFind::AtPoint(const position_t& pos)
{
    // Dual-wire: pathfindstatushelpers::AtPointThreshold (slice 6337).
    const float threshold = pathfindstatushelpers::AtPointThreshold(m_distanceFromPoint);
    return isWithinDistance(m_POwner->loc.p, pos, threshold);
}

bool CPathFind::InWater()
{
    const auto& pos     = m_POwner->loc.p;
    const auto  terrain = m_POwner->loc.zone->xiMesh()->getTerrainAt(pos.x, pos.y, pos.z);
    // Dual-wire: pathfindstatushelpers::IsWaterTerrain (slice 6338).
    return pathfindstatushelpers::IsWaterTerrain(
        terrain == TerrainType::ShallowWater,
        terrain == TerrainType::DeepWater);
}

const position_t& CPathFind::GetDestination() const
{
    // Dual-wire: pathfindstatushelpers::HasDestination pure half (slice 6346)
    // documents non-empty precondition. Production assumes points are present
    // (same as upstream); callers may guard with HasDestination(size).
    return m_points.back().position;
}

void CPathFind::SetCarefulPathing(bool careful)
{
    // Dual-wire: pathfindstatushelpers::CarefulPathingValue (slice 6343).
    m_carefulPathing = pathfindstatushelpers::CarefulPathingValue(careful);
}

void CPathFind::Clear()
{
    // Dual-wire: pathfindstatushelpers::Cleared* sticky defaults (slice 6349).
    m_distanceFromPoint = pathfindstatushelpers::ClearedDistanceFromPoint();
    m_pathFlags         = pathfindstatushelpers::ClearedPathFlags();
    m_roamFlags         = pathfindstatushelpers::ClearedRoamFlags();

    m_points.clear();

    // Dual-wire: ClearedWaiting (slice 6354). False means assign inactive wait deadline.
    if (!pathfindstatushelpers::ClearedWaiting())
    {
        m_timeAtPoint = timer::time_point::min();
    }

    m_currentPoint  = pathfindstatushelpers::ClearedCurrentPoint();
    m_maxDistance   = pathfindstatushelpers::ClearedMaxDistance();
    m_distanceMoved = pathfindstatushelpers::ClearedDistanceMoved();

    m_onPoint = pathfindstatushelpers::ClearedOnPoint();

    m_currentTurn = pathfindstatushelpers::ClearedCurrentTurn();
    m_turnPoints.clear();
}

void CPathFind::AddPoints(std::vector<pathpoint_t>&& points, bool reverse)
{
    // Dual-wire: pathfindstatushelpers::ShouldTruncatePathPoints (slice 6339).
    if (pathfindstatushelpers::ShouldTruncatePathPoints(points.size(), MAX_PATH_POINTS, (m_pathFlags & PATHFLAG_PATROL) != 0))
    {
        ShowWarning("CPathFind::AddPoints Given too many points (%d). Limiting to max (%d)", points.size(), MAX_PATH_POINTS);
        points.resize(MAX_PATH_POINTS);
    }

    m_points = std::move(points);

    if (reverse)
    {
        std::reverse(m_points.begin(), m_points.end());
    }

    // Dual-wire: ShouldSnapshotPatrol / PatrolFlagsValue / ClearedPatrolFlags (slice 6351).
    if (pathfindstatushelpers::ShouldSnapshotPatrol((m_pathFlags & PATHFLAG_PATROL) != 0))
    {
        m_patrol      = m_points;
        m_patrolFlags = pathfindstatushelpers::PatrolFlagsValue(m_pathFlags);
    }
    else
    {
        m_patrol.clear();
        m_patrolFlags = pathfindstatushelpers::ClearedPatrolFlags();
    }
}

void CPathFind::FinishedPath()
{
    m_currentTurn++;

    const auto action = pathfindfinishedhelpers::Resolve(m_currentTurn < m_turnPoints.size(), IsPatrolling(), m_POwner->PAI->IsRoaming());

    if (action == pathfindfinishedhelpers::Action::NextTurn)
    {
        // move on to next turn
        position_t& nextTurn = m_turnPoints[m_currentTurn];

        bool result = FindPath(m_POwner->loc.p, nextTurn);

        // Dual-wire: pathfindstatushelpers::ShouldClearAfterFailedPath (slice 6348).
        if (pathfindstatushelpers::ShouldClearAfterFailedPath(result))
        {
            Clear();
        }
    }
    else if (action == pathfindfinishedhelpers::Action::RestartPatrol)
    {
        // Dual-wire: RestartedCurrentPoint / RestartedCurrentTurn (slice 6351).
        m_currentPoint = pathfindstatushelpers::RestartedCurrentPoint();
        m_currentTurn  = pathfindstatushelpers::RestartedCurrentTurn();
    }
    else
    {
        Clear();
    }
}
