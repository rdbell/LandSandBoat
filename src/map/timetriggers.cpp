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

#include "timetriggers.h"

#include "common/vana_time.h"

#include "lua/luautils.h"

void CTriggerHandler::insertTrigger(Trigger_t trigger)
{
    insertTriggerAt(trigger, vanadiel_time::now().time_since_epoch());
}

void CTriggerHandler::insertTriggerAt(Trigger_t trigger, vanadiel_time::duration vanaTime)
{
    vanadiel_time::duration alignedTime = vanaTime - trigger.minuteOffset;
    trigger.lastTrigger                 = alignedTime / trigger.period;
    triggerList.emplace_back(trigger);
}

void CTriggerHandler::triggerTimer()
{
    triggerTimerAt(vanadiel_time::now().time_since_epoch(), [](CNpcEntity* npc, uint8 id)
    {
        luautils::OnTimeTrigger(npc, id);
    });
}

void CTriggerHandler::triggerTimerAt(vanadiel_time::duration vanaTime, const TriggerCallback& callback)
{
    Trigger_t*              trigger = nullptr;
    vanadiel_time::duration alignedTime;
    uint32                  timeCount = 0;

    for (auto& i : triggerList)
    {
        trigger     = &i;
        alignedTime = vanaTime - trigger->minuteOffset;
        timeCount   = alignedTime / trigger->period;

        if (timeCount > trigger->lastTrigger)
        {
            callback(trigger->npc, trigger->id);
            trigger->lastTrigger = timeCount;
        }
    }
}
