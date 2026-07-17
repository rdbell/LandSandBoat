#pragma once

#include "common/cbasetypes.h"

// Pure Manaclipper helpers shared by dual-wire slices:
//   - 2887: RemainingGameMinutes (timekeeperOnTrigger game-minute countdown)
//
// Lua production host: scripts/globals/manaclipper.lua
//   xi.manaclipper.timekeeperOnTrigger (~101–133):
//
//     local gameMins = nextEvent.endTime - currentTime
//     if nextEvent.endTime < currentTime then
//         -- next event is before current time because it's near the end of the day
//         gameMins = 1440 + nextEvent.endTime - currentTime
//     end
//
// Host injects scalars only (no player / schedule pointers):
//   currentTime — VanadielHour()*60 + VanadielMinute()
//   endTime     — nextEvent.endTime (minutes past midnight)
//
// getNextEvent selection, startEvent, earthSecs conversion remain host-owned.
// Prior pure port: OmegaXI slice 0942 (internal/manaclipper).
// Dual-wire of Go manaclipper.RemainingGameMinutes (slice 2887).

namespace manaclipperhelpers
{

// MinutesPerGameDay is the Vana'diel day length in minutes used for day-wrap
// countdown (matches transports.sql day alignment and Lua's literal 1440).
inline constexpr int32 kMinutesPerGameDay = 1440;

// RemainingGameMinutes is the pure game-minute countdown from
// timekeeperOnTrigger once currentTime and nextEvent.endTime are injected:
//
//   gameMins = endTime - currentTime
//   if endTime < currentTime:
//     gameMins = kMinutesPerGameDay + endTime - currentTime
//
// Dual-wire of Go manaclipper.RemainingGameMinutes / Lua lines ~111–116.
inline auto RemainingGameMinutes(const int32 currentTime, const int32 endTime) -> int32
{
    int32 gameMins = endTime - currentTime;
    if (endTime < currentTime)
    {
        // next event is before current time (near end of day); add a cycle
        gameMins = kMinutesPerGameDay + endTime - currentTime;
    }
    return gameMins;
}

} // namespace manaclipperhelpers
