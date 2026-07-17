#pragma once

#include "common/cbasetypes.h"

// Pure Salvage helpers shared by dual-wire slices:
//   - 2871: CanClaimTransport / TransportUserBusy residual dual-wire suite
//   - 3085: CanClaimTransport dedicated dual-wire (claim_transport.go)
//   - 2892: CanOpenDoor residual dual-wire suite (onDoorOpen CLOSE_DOOR + unSealed)
//   - 3133: CanOpenDoor dedicated dual-wire (open_door.go)
//   - 2894: CanOpenBossDoor (openBossDoor CLOSE_DOOR gate; no unSealed)
//   - 2898: ShouldResetTempBox residual dual-wire suite (resetTempBoxes NORMAL)
//   - 3146: ShouldResetTempBox dedicated dual-wire (reset_temp_box.go)
//   - 2904: ShouldSpawnOnTempChestCasket (spawnTempChest status == DISAPPEAR)
//
// Dual-wire index:
//   - 2871: CanClaimTransport residual dual-wire suite
//   - 3085: CanClaimTransport = !TransportUserBusy(transportUserID)
//   - 2892: CanOpenDoor residual dual-wire suite
//   - 3133: CanOpenDoor = animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue
//   - 2894: CanOpenBossDoor
//   - 2898: ShouldResetTempBox residual dual-wire suite
//   - 3146: ShouldResetTempBox = status == kStatusNormal
//   - 2904: ShouldSpawnOnTempChestCasket
//
// Lua production host: scripts/globals/salvage.lua
// Go dual-wire: salvage.CanClaimTransport / salvage.TransportUserBusy
// (internal/salvage/claim_transport.go); salvage.CanOpenDoor
// (internal/salvage/open_door.go); salvage.ShouldResetTempBox
// (internal/salvage/reset_temp_box.go). Future Lua host injects free
// functions then claim/open/reset writeback.
//
// Prior pure ports: OmegaXI slices 0977 (TransportUserBusy / CanOpenDoor),
// 1083 (CanClaimTransport / DoorUnsealedValue / ShouldResetTempBox). Residual
// dual-wire suites: 2871 (claim), 2892 (open door), 2898 (reset temp box).
// Dedicated dual-wire: 3085, 3133, 3146.
//
// onTransportUpdate (2871 residual / 3085 dedicated):
//   if instance:getLocalVar('transportUser') == 0 then
//     -- claim path: set transportUser, stageComplete=0, resetTempBoxes, ...
//   else
//     return
//   end
//
// onDoorOpen (2892 residual / 3133 dedicated):
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
// resetTempBoxes (2898 residual / 3146 dedicated):
//   if casket and casket:getStatus() == xi.status.NORMAL then
//     -- host: setStatus(DISAPPEAR), resetLocalVars, setAnimationSub(8)
//   end
//
// spawnTempChest (2904):
//   if casket and casket:getStatus() == xi.status.DISAPPEAR then
//     -- host: setPos(mob), resetLocalVars, setStatus(NORMAL), prePicked/items
//   end
//
// Host injects scalars only (no instance / npc pointers). Claim writeback,
// door open writeback, temp-box reset writeback, and temp-chest spawn
// writeback remain host-owned. Future Lua/C++ hosts dual-wire these free
// functions instead of re-inlining the comparisons.

namespace salvagehelpers
{

// ---------------------------------------------------------------------------
// Slice 2871 / 3085 — onTransportUpdate transportUser claim gate
// ---------------------------------------------------------------------------

// TransportUserBusy is the pure free-function form of the busy half of the
// onTransportUpdate gate: another player already holds transportUser.
//   transportUserID != 0
// Prior pure port: slice 0977. Residual dual-wire suite: 2871.
inline auto TransportUserBusy(const uint32 transportUserID) -> bool
{
    return transportUserID != 0;
}

// CanClaimTransport is the pure free-function form of the onTransportUpdate
// claim gate: transportUser local var must be free before the claim path runs.
//
// Formula (slice 3085 dedicated dual-wire; residual expand 2871 / pure 1083 —
// formula unchanged):
//   CanClaimTransport(transportUserID) = !TransportUserBusy(transportUserID)
//   ≡ transportUserID == 0
//
// Host injects transportUser (getLocalVar) only. Free (0) may claim; non-zero
// is busy → early return.
// Dual-wire of Go salvage.CanClaimTransport (claim_transport.go).
// Call site: future Lua onTransportUpdate inject.
// Prior pure port: slice 1083. Residual dual-wire suite: 2871 /
// test_salvage_claim_transport_2871. Dedicated dual-wire suite is
// test_salvage_claim_transport_3085. Host still owns claim writeback,
// stageComplete reset, timer clear, deSpawnStage, release, resetTempBoxes.
inline auto CanClaimTransport(const uint32 transportUserID) -> bool
{
    return !TransportUserBusy(transportUserID);
}

// ---------------------------------------------------------------------------
// 2892 residual / 3133 dedicated — onDoorOpen CLOSE_DOOR + unSealed gate
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
// Formula (slice 3133 dedicated dual-wire; residual expand 2892 / pure 0977 /
// 1083 — formula unchanged):
//   CanOpenDoor(animation, unSealed) =
//     animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue
//   ≡ animation == CLOSE_DOOR (9) && unSealed == 1
//
// Host injects npc:getAnimation() and npc:getLocalVar('unSealed') only.
// Host still owns setLocalVar('unSealed', 0), optional setStage/setProgress,
// setAnimation(OPEN_DOOR), and setUntargetable(true).
// Dual-wire of Go salvage.CanOpenDoor (open_door.go).
// Call site: future Lua onDoorOpen inject.
// Prior pure port: slice 0977. Residual dual-wire suite: 2892 /
// test_salvage_open_door_2892. Dedicated dual-wire suite is
// test_salvage_can_open_door_3133.
inline auto CanOpenDoor(const uint8 animation, const int32 unSealed) -> bool
{
    return animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue;
}

// ---------------------------------------------------------------------------
// 2898 residual / 3146 dedicated — resetTempBoxes status == NORMAL gate
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
// Formula (slice 3146 dedicated dual-wire; residual expand 2898 / pure 1083 —
// formula unchanged):
//   ShouldResetTempBox(status) = status == kStatusNormal
//   ≡ status == NORMAL (0)
//
// Host injects casket:getStatus() only. Host still owns setStatus(DISAPPEAR),
// resetLocalVars, and setAnimationSub(8). Dual-wire of Go
// salvage.ShouldResetTempBox (reset_temp_box.go).
// Call site: future Lua resetTempBoxes inject.
// Prior pure port: slice 1083. Residual dual-wire suite: 2898 /
// test_salvage_reset_temp_box_2898. Dedicated dual-wire suite is
// test_salvage_reset_temp_box_3146.
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

// ---------------------------------------------------------------------------
// 2904 — spawnTempChest status == DISAPPEAR gate
// ---------------------------------------------------------------------------

// ShouldSpawnOnTempChestCasket is the pure free-function form of the
// spawnTempChest casket-search status gate:
//
//   status == DISAPPEAR
//   ≡ status == kStatusDisappear
//
// Host injects casket:getStatus() only. Host still owns setPos from the
// dead mob, resetLocalVars, setStatus(NORMAL), and optional prePicked /
// itemID_1 locals. Dual-wire of Go salvage.ShouldSpawnOnTempChestCasket
// (slice 2904 / residual 1083).
// Status pins: kStatusNormal / kStatusDisappear (see 2898 block above).
inline auto ShouldSpawnOnTempChestCasket(const uint8 status) -> bool
{
    return status == kStatusDisappear;
}

} // namespace salvagehelpers
