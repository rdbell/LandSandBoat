#pragma once

#include "common/cbasetypes.h"

// Pure Salvage helpers shared by dual-wire slices:
//   - 2871: CanClaimTransport / TransportUserBusy (onTransportUpdate gate)
//   - 2892: CanOpenDoor (onDoorOpen CLOSE_DOOR + unSealed gate)
//
// Lua production host: scripts/globals/salvage.lua
//
// onTransportUpdate (2871):
//   if instance:getLocalVar('transportUser') == 0 then
//     -- claim path: set transportUser, stageComplete=0, resetTempBoxes, ...
//   else
//     return
//   end
//
// onDoorOpen (2892):
//   if
//       npc:getAnimation() == xi.animation.CLOSE_DOOR and
//       npc:getLocalVar('unSealed') == 1
//   then
//     -- host: clear unSealed, optional setStage/setProgress, OPEN_DOOR, untargetable
//   end
//
// Host injects scalars only (no instance / npc pointers). Claim writeback and
// door open writeback remain host-owned. Future Lua/C++ hosts dual-wire these
// free functions instead of re-inlining the comparisons.

namespace salvagehelpers
{

// ---------------------------------------------------------------------------
// 2871 — onTransportUpdate transportUser claim gate
// ---------------------------------------------------------------------------

// TransportUserBusy is the pure free-function form of the busy half of the
// onTransportUpdate gate: another player already holds transportUser.
//   transportUserID != 0
inline auto TransportUserBusy(const uint32 transportUserID) -> bool
{
    return transportUserID != 0;
}

// CanClaimTransport is the pure free-function form of the onTransportUpdate
// claim gate: transportUser local var must be free before the claim path runs.
//   transportUserID == 0
// Equivalent to !TransportUserBusy(transportUserID).
inline auto CanClaimTransport(const uint32 transportUserID) -> bool
{
    return transportUserID == 0;
}

// ---------------------------------------------------------------------------
// 2892 — onDoorOpen CLOSE_DOOR + unSealed gate
// ---------------------------------------------------------------------------

// Door animation / seal pins (xi.animation / sealDoors / unsealDoors):
//   CLOSE_DOOR = 9 — required animation for onDoorOpen / openBossDoor
//   OPEN_DOOR  = 8 — residual pin (already open → gate fails)
//   unSealed   = 0 sealed (sealDoors) / 1 unsealed (unsealDoors)
// CanOpenDoor requires CLOSE_DOOR and unSealed == 1.
inline constexpr uint8 kAnimOpenDoor      = 8;
inline constexpr uint8 kAnimCloseDoor     = 9;
inline constexpr int32 kDoorSealedValue   = 0;
inline constexpr int32 kDoorUnsealedValue = 1;

// CanOpenDoor is the pure free-function form of the onDoorOpen gate:
//
//   animation == CLOSE_DOOR && unSealed == 1
//   ≡ animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue
//
// Host injects npc:getAnimation() and npc:getLocalVar('unSealed') only.
// Host still owns setLocalVar('unSealed', 0), optional setStage/setProgress,
// setAnimation(OPEN_DOOR), and setUntargetable(true).
// Dual-wire of Go salvage.CanOpenDoor (slice 2892 / residual 0977).
inline auto CanOpenDoor(const uint8 animation, const int32 unSealed) -> bool
{
    return animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue;
}

} // namespace salvagehelpers
