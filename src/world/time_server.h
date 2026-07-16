/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#pragma once

#include <common/scheduler.h>

class WorldEngine;

// TimeServerTickEffects receives the side effects selected by one world time
// server poll. Keeping this boundary independent of WorldEngine makes the
// time-server's dispatch rules directly testable.
class TimeServerTickEffects
{
public:
    virtual ~TimeServerTickEffects() = default;

    virtual void updateWeeklyConquest()     = 0;
    virtual void updateHourlyConquest()     = 0;
    virtual void updateDailyTally()         = 0;
    virtual void updateVanaHourlyConquest() = 0;
};

// TimeServerTickInput contains the already-classified hourly boundaries and
// calendar fields used by the world time server.
struct TimeServerTickInput
{
    bool earthHourlyTick = false;
    int  jstHour         = 0;
    int  jstWeekday      = 0;
    bool vanaHourlyTick  = false;
};

// dispatchTimeServerTickEffects maps classified earth and Vana hourly ticks to
// their observable effects. Earth effects always dispatch before Vana effects.
void dispatchTimeServerTickEffects(const TimeServerTickInput& input, TimeServerTickEffects& effects);

auto time_server(const WorldEngine* worldServer) -> Task<void>;
