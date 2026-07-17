#pragma once

#include "common/cbasetypes.h"

// Pure Manaclipper helpers shared by dual-wire slices:
//   - 0942: residual pure port (internal/manaclipper RemainingGameMinutes /
//           ArrivalEventID / schedule)
//   - 2887: RemainingGameMinutes residual dual-wire suite
//           (remaining_game_minutes_test.go / test_manaclipper_remaining_minutes_2887)
//   - 3162: RemainingGameMinutes dedicated dual-wire
//           (remaining_game_minutes.go / test_manaclipper_remaining_game_minutes_3162)
//   - 2897: ArrivalEventID (onZoneIn route → Bibiki arrival CSID)
//
// Dual-wire index:
//   - 2887: RemainingGameMinutes residual dual-wire suite
//   - 3162: RemainingGameMinutes (end-current / +1440 wrap) dedicated suite
//   - 2897: ArrivalEventID (route → 13 / 12)
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
//   xi.manaclipper.onZoneIn (~138–148), zoning onto MANACLIPPER:
//
//     if nextEvent.route == dest.PURGONORGO_ISLE then
//         player:setCharVar('[manaclipper]arrivalEventId', 13)
//     else
//         player:setCharVar('[manaclipper]arrivalEventId', 12)
//     end
//
// Host injects scalars only (no player / schedule pointers):
//   currentTime / endTime — Vanadiel minutes past midnight / nextEvent.endTime
//   route                 — nextEvent.route (dest enum 0..3)
//
// getNextEvent selection, startEvent, earthSecs conversion, charvar write,
// and Bibiki return path remain host-owned.
// Prior pure port: OmegaXI slice 0942 (internal/manaclipper).
// Dual-wire of Go manaclipper.RemainingGameMinutes (slice 3162 dedicated;
// residual expand 2887 / pure 0942).
// Dual-wire of Go manaclipper.ArrivalEventID (slice 2897; left alone under 3162).
// Residual dual-wire suite: 2887 (test_manaclipper_remaining_minutes_2887).
// Dedicated dual-wire suite: 3162 (test_manaclipper_remaining_game_minutes_3162).

namespace manaclipperhelpers
{

// MinutesPerGameDay is the Vana'diel day length in minutes used for day-wrap
// countdown (matches transports.sql day alignment and Lua's literal 1440).
// Residual constant shared by RemainingGameMinutes (3162 / 2887 / 0942).
inline constexpr int32 kMinutesPerGameDay = 1440;

// Dest enum pins (local dest table in manaclipper.lua / Go Dest).
inline constexpr int32 kDestDhalmelRock      = 0;
inline constexpr int32 kDestMaliyakaleyaReef = 1;
inline constexpr int32 kDestPurgonorgoIsle   = 2;
inline constexpr int32 kDestSunsetDocks      = 3;

// Bibiki Bay arrival CSIDs written to [manaclipper]arrivalEventId.
// 13 → set pos then chain to 11 (arrive Purgonorgo Isle)
// 12 → set pos then chain to 10 (arrive Sunset Docks)
inline constexpr int32 kArrivalEventPurgonorgoIsle = 13;
inline constexpr int32 kArrivalEventSunsetDocks    = 12;

// RemainingGameMinutes is the pure game-minute countdown from
// timekeeperOnTrigger once currentTime and nextEvent.endTime are injected:
//
//   gameMins = endTime - currentTime
//   if endTime < currentTime:
//     gameMins = kMinutesPerGameDay + endTime - currentTime
//
// Formula (slice 3162 dedicated dual-wire; residual expand 2887 / pure 0942 —
// formula unchanged):
//   RemainingGameMinutes(currentTime, endTime)
//     gameMins = endTime - currentTime
//     if endTime < currentTime:
//       gameMins = kMinutesPerGameDay + endTime - currentTime
//     return gameMins
//
// Dual-wire of Go manaclipper.RemainingGameMinutes.
// Call site: future Lua timekeeperOnTrigger inject (countdown half).
// Prior pure port: slice 0942. Residual dual-wire suite: 2887 /
// test_manaclipper_remaining_minutes_2887. Dedicated dual-wire suite is
// test_manaclipper_remaining_game_minutes_3162. Host still owns getNextEvent,
// Vanadiel clock, earthSecs conversion, and startEvent writeback.
// Future Lua host injects scalars into this helper instead of re-inlining
// the wrap arithmetic.
// Sibling ArrivalEventID is left alone under 3162 (residual 2897).
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

// ArrivalEventID is the pure onZoneIn half for zoning onto MANACLIPPER once
// nextEvent.route is injected:
//
//   route == PURGONORGO_ISLE → 13, else → 12
//
// Sibling residual only under slice 3162 (dedicated RemainingGameMinutes
// expand); dual-wire surface remains residual 2897 / pure 0942. Formula
// unchanged.
// Dual-wire of Go manaclipper.ArrivalEventID / Lua lines ~144–148.
inline auto ArrivalEventID(const int32 route) -> int32
{
    if (route == kDestPurgonorgoIsle)
    {
        return kArrivalEventPurgonorgoIsle;
    }
    return kArrivalEventSunsetDocks;
}

} // namespace manaclipperhelpers
