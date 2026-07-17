#pragma once

#include "common/cbasetypes.h"

// Pure Salvage helpers shared by dual-wire slices:
//   - 2871: CanClaimTransport / TransportUserBusy (onTransportUpdate gate)
//   - 2892: CanOpenDoor (onDoorOpen CLOSE_DOOR + unSealed gate)
//   - 2894: CanOpenBossDoor (openBossDoor CLOSE_DOOR gate; no unSealed)
//   - 2898: ShouldResetTempBox (resetTempBoxes status == NORMAL gate)
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
// openBossDoor (2894):
//   if npc:getAnimation() == xi.anim.CLOSE_DOOR then
//     -- host: openDoor(15), queue(3000) arch openDoor(10)
//   end
//
// resetTempBoxes (2898):
//   if casket and casket:getStatus() == xi.status.NORMAL then
//     -- host: setStatus(DISAPPEAR), resetLocalVars, setAnimationSub(8)
//   end
//
// Host injects scalars only (no instance / npc pointers). Claim writeback,
// door open writeback, and temp-box reset writeback remain host-owned.
// Future Lua/C++ hosts dual-wire these free functions instead of re-inlining
// the comparisons.

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

// ---------------------------------------------------------------------------
// 2898 — resetTempBoxes status == NORMAL gate
// ---------------------------------------------------------------------------

// Entity status pins (xi.status.*) used by crate / temp-box reset / spawn:
//   NORMAL    = 0 — required status for resetTempBoxes writeback
//   DISAPPEAR = 2 — residual pin (already disappeared → gate fails; also
//                   spawnTempChest target status)
// ShouldResetTempBox requires status == NORMAL.
inline constexpr uint8 kStatusNormal    = 0;
inline constexpr uint8 kStatusDisappear = 2;

// ShouldResetTempBox is the pure free-function form of the resetTempBoxes
// status gate:
//
//   status == NORMAL
//   ≡ status == kStatusNormal
//
// Host injects casket:getStatus() only. Host still owns setStatus(DISAPPEAR),
// resetLocalVars, and setAnimationSub(8). Dual-wire of Go
// salvage.ShouldResetTempBox (slice 2898 / residual 1083).
inline auto ShouldResetTempBox(const uint8 status) -> bool
{
    return status == kStatusNormal;
}

// ---------------------------------------------------------------------------
// 2894 — openBossDoor CLOSE_DOOR gate (no unSealed)
// ---------------------------------------------------------------------------

// CanOpenBossDoor is the pure free-function form of the openBossDoor gate:
//
//   animation == CLOSE_DOOR
//   ≡ animation == kAnimCloseDoor
//
// Host injects npc:getAnimation() only. Unlike CanOpenDoor there is no
// unSealed check. Host still owns openDoor(15), queue(3000), and arch NPC
// openDoor(10) via bossDoorID - 1.
// Dual-wire of Go salvage.CanOpenBossDoor (slice 2894 / residual 0977).
inline auto CanOpenBossDoor(const uint8 animation) -> bool
{
    return animation == kAnimCloseDoor;
}

} // namespace salvagehelpers
