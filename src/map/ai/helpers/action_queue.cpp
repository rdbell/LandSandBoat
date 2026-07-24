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

#include "action_queue.h"
#include "ai/ai_container.h"
#include "ai/helpers/action_queue_capacity.h"
#include "entities/base_entity.h"
#include "lua/lua_base_entity.h"
#include "lua/luautils.h"

CAIActionQueue::CAIActionQueue(CBaseEntity* _PEntity)
: PEntity(_PEntity)
{
}

void CAIActionQueue::pushAction(queueAction_t&& action)
{
    // Dual-wire: actionqueuehelpers::ShouldRouteToActionQueue (slice 6333).
    if (actionqueuehelpers::ShouldRouteToActionQueue(action.checkState))
    {
        actionQueue.push(std::move(action));
    }
    else
    {
        timerQueue.push(std::move(action));
    }
}

void CAIActionQueue::checkAction(timer::time_point tick)
{
    while (!timerQueue.empty())
    {
        const auto& topaction = timerQueue.top();
        // Dual-wire: actionqueuehelpers::ActionDueStrict (slice 6332).
        // Timer queue entries use checkState=false; state gate always allows.
        if (actionqueuehelpers::ActionDueStrict(tick, topaction.start_time + topaction.delay) &&
            actionqueuehelpers::ActionStateGateAllows(topaction.checkState, true))
        {
            queueAction_t action = timerQueue.top();
            timerQueue.pop();
            handleAction(action);
        }
        else
        {
            break;
        }
    }
    while (!actionQueue.empty())
    {
        const auto& topaction = actionQueue.top();
        // Dual-wire: ActionDueStrict (6332) + ActionStateGateAllows (6333).
        if (actionqueuehelpers::ActionDueStrict(tick, topaction.start_time + topaction.delay) &&
            actionqueuehelpers::ActionStateGateAllows(topaction.checkState, PEntity->PAI->CanChangeState()))
        {
            auto action = actionQueue.top();
            actionQueue.pop();
            handleAction(action);
        }
        else
        {
            break;
        }
    }
}

void CAIActionQueue::handleAction(queueAction_t& action)
{
    if (action.lua_func.valid())
    {
        auto result = action.lua_func(PEntity);
        if (!result.valid())
        {
            sol::error err = result;
            ShowError("CAIActionQueue::handleAction for %s (%i): %s", PEntity->name, PEntity->id, err.what());
        }
    }

    if (action.func)
    {
        action.func(PEntity);
    }
}

bool CAIActionQueue::isEmpty()
{
    // Dual-wire: actionqueuehelpers::IsBothQueuesEmpty (slice 6331).
    return actionqueuehelpers::IsBothQueuesEmpty(actionQueue.empty(), timerQueue.empty());
}

void CAIActionQueue::clearActionQueue()
{
    // Dual-wire: actionqueuehelpers::ShouldContinueClear (slice 6334).
    while (actionqueuehelpers::ShouldContinueClear(actionQueue.empty()))
    {
        actionQueue.pop();
    }
}

void CAIActionQueue::clearTimerQueue()
{
    // Dual-wire: actionqueuehelpers::ShouldContinueClear (slice 6334).
    while (actionqueuehelpers::ShouldContinueClear(timerQueue.empty()))
    {
        timerQueue.pop();
    }
}
