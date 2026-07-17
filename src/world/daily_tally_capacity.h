/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

// Pure daily-tally schedule helpers (slice 2856).
//
// Production host: dispatchTimeServerTickEffects / time_server in
// time_server.cpp dual-wires ShouldRunDailyTallyOnHourlyTick for the
// midnight JST daily-tally branch.
//
// Helpers take host-injected scalars only (no earth_time, settings, or DB).

namespace dailytallyhelpers
{

// IsJSTDailyTickHour reports whether jstHour is the midnight daily-tick hour.
// Mirrors time_server's `jstHour == 0` midnight branch.
inline auto IsJSTDailyTickHour(const int jstHour) -> bool
{
    return jstHour == 0;
}

// ShouldRunDailyTallyOnHourlyTick is the pure schedule gate from time_server:
// when an Earth JST hourly tick is due and the JST hour is 0 (midnight), the
// host invokes dailytally::UpdateDailyTallyPoints.
//
//   hourlyTickDue && jstHour == 0
//
// LSB production (dispatchTimeServerTickEffects):
//   if (earthHourlyTick) {
//       if (jstHour == 0) {
//           ...
//           effects.updateDailyTally();
//       }
//   }
inline auto ShouldRunDailyTallyOnHourlyTick(const bool hourlyTickDue, const int jstHour) -> bool
{
    return hourlyTickDue && IsJSTDailyTickHour(jstHour);
}

} // namespace dailytallyhelpers
