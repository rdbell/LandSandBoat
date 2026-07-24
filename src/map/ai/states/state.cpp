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

#include "state.h"
#include "entities/base_entity.h"
#include "ai/states/state_complete.h"
#include "ai/states/state_delay_exit.h"
#include "ai/states/state_error_msg.h"
#include "ai/states/state_is_completed.h"
#include "ai/states/state_set_target.h"
#include "ai/states/state_was_exit_delayed.h"

CState::CState(CBaseEntity* PEntity, uint16 _targid)
: m_PEntity(PEntity)
, m_targid(_targid)
{
    // TODO: determine if this should go here;
    // m_PTarget = m_PEntity->GetEntity(_targid);
}

void CState::UpdateTarget(uint16 targid)
{
    m_PTarget = m_PEntity->GetEntity(targid);
}

void CState::UpdateTarget(CBaseEntity* target)
{
    m_PTarget = target;
}

CBaseEntity* CState::GetTarget() const
{
    return m_PTarget;
}

uint16 CState::GetTargetID() const
{
    return m_targid;
}

void CState::Complete()
{
    // Dual-wire: statehelpers::MarkCompleted (slice 6328).
    m_completed = statehelpers::MarkCompleted();
}

timer::time_point CState::GetEntryTime() const
{
    return m_entryTime;
}

bool CState::WasExitDelayed()
{
    // Dual-wire: statehelpers::WasExitDelayed (slice 6326).
    return statehelpers::WasExitDelayed(m_wasDelayed);
}

void CState::DelayExitTime(std::chrono::milliseconds delayMilliseconds)
{
    // Dual-wire: statehelpers::delayExitPlan (slice 6320).
    const auto plan = statehelpers::delayExitPlan(m_entryTime, delayMilliseconds);
    m_entryTime     = plan.entryTime;
    m_wasDelayed    = plan.wasDelayed;
}

void CState::ResetEntryTime()
{
    m_entryTime = timer::now();
}

void CState::SetTarget(uint16 _targid)
{
    const bool   hasTarget     = m_PTarget != nullptr;
    const uint16 entityTargid  = hasTarget ? m_PTarget->targid : 0;
    // Dual-wire: statehelpers::ShouldUpdateTarget (slice 6317).
    if (statehelpers::ShouldUpdateTarget(hasTarget, m_targid, _targid, entityTargid))
    {
        m_targid = _targid;
        UpdateTarget(_targid);
    }
}

bool CState::HasErrorMsg() const
{
    // Dual-wire: statehelpers::HasErrorMsg (slice 6324).
    return statehelpers::HasErrorMsg(m_errorMsg != nullptr);
}

auto CState::GetErrorMsg() -> std::unique_ptr<CBasicPacket>
{
    // Dual-wire residual: reuses HasErrorMsg free gate (slice 6324).
    if (statehelpers::HasErrorMsg(m_errorMsg != nullptr))
    {
        return m_errorMsg->copy();
    }

    ShowError("State attempted to get error message when error message was null");

    return std::unique_ptr<CBasicPacket>();
}

bool CState::DoUpdate(timer::time_point tick)
{
    UpdateTarget(m_targid);
    return Update(tick);
}

bool CState::IsCompleted() const
{
    // Dual-wire: statehelpers::IsCompleted (slice 6325).
    return statehelpers::IsCompleted(m_completed);
}
