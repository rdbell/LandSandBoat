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

#include "trigger_state.h"

#include "ai/ai_container.h"
#include "ai/states/trigger_update.h"
#include "entities/char_entity.h"
#include "entities/npc_entity.h"
#include "lua/luautils.h"

CTriggerState::CTriggerState(CBaseEntity* PEntity, uint16 targid, bool door)
: CState(PEntity, targid)
, door(door)
{
}

bool CTriggerState::Update(timer::time_point tick)
{
    if (!IsCompleted())
    {
        auto*      PChar           = dynamic_cast<CCharEntity*>(GetTarget());
        const bool hasCharTarget   = PChar != nullptr;
        const bool isCloseDoorAnim = m_PEntity->animation == ANIMATION_CLOSE_DOOR;
        // Door open admission (slice 6319 dual-wire).
        if (triggerupdate::shouldOpenDoor(hasCharTarget, door, isCloseDoorAnim))
        {
            close                = true;
            m_PEntity->animation = ANIMATION_OPEN_DOOR;
            m_PEntity->updatemask |= UPDATE_HP;
        }
        Complete();
    }
    else if (close)
    {
        const bool tickAfterEntryPlus7s = tick > GetEntryTime() + 7s;
        // Door re-close admission (slice 6319 dual-wire).
        if (triggerupdate::shouldCloseDoor(close, tickAfterEntryPlus7s))
        {
            m_PEntity->animation = ANIMATION_CLOSE_DOOR;
            m_PEntity->updatemask |= UPDATE_HP;
            return true;
        }
    }
    else if (triggerupdate::shouldExitImmediate(IsCompleted(), close))
    {
        // Non-door / no re-close pending: leave immediately (slice 6319 dual-wire).
        return true;
    }
    return false;
}

bool CTriggerState::CanChangeState()
{
    return false;
}

bool CTriggerState::CanFollowPath()
{
    return false;
}
