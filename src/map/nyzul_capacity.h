#pragma once

#include "common/cbasetypes.h"

// Pure Nyzul Isle helpers shared by dual-wire slices:
//   - 2874: free-floor selection gate (pickSetPoint; prior dual-wire)
//   - 2891: gear-objective chance gate (pickSetPoint)
//   - 2900: floor-100 vigil weapon drop gate (vigilWeaponDrop; residual dual-wire)
//   - 2902: Rune of Transfer first-claimer gate (onEventUpdate; residual dual-wire)
//   - 2905: spawnChest regular-mob casket roll (prior dual-wire)
//   - 2909: non-floor-100 NM vigil weapon roll (vigilWeaponDrop)
//   - 2913: activateRuneOfTransfer NORMAL status gate (prior dual-wire)
//   - 2914: clearChests present + not-DISAPPEAR status gate (prior dual-wire)
//   - 2918: onGearEngage AVOID_AGRO penalty-trigger gate (pathos)
//   - 3061: clearChests present + not-DISAPPEAR status gate (ShouldClearChestNPC)
//   - 3095: free-floor selection gate (ShouldGrantFreeFloor / pickSetPoint)
//   - 3110: activateRuneOfTransfer NORMAL status gate (ShouldActivateRuneOfTransfer)
//   - 3128: spawnChest regular-mob casket roll (ShouldSpawnCasket)
//   - 3240: CanClaimRuneHandler prior dedicated dual-wire expand residual 2902
//           (retained; formula unchanged: runeHandler == 0)
//   - 3281: CanClaimRuneHandler prior dedicated dual-wire expand residual 2902
//           (retained; prior dedicated 3240; formula unchanged: runeHandler == 0)
//   - 3311: CanClaimRuneHandler dedicated dual-wire expand residual 2902
//           (prior dedicated 3281; formula unchanged: runeHandler == 0)
//   - 3352: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; formula unchanged: currentFloor == Floor100)
//   - 3449: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352; formula unchanged: currentFloor == Floor100)
//   - 3502: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449; formula unchanged: currentFloor == Floor100)
//   - 3570: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502; formula unchanged: currentFloor == Floor100)
//   - 3615: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570; formula unchanged: currentFloor == Floor100)
//   - 3660: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615; formula unchanged: currentFloor == Floor100)
//   - 3705: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660; formula unchanged: currentFloor == Floor100)
//   - 3750: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660 / 3705; formula unchanged: currentFloor == Floor100)
//   - 3795: ShouldDropFloor100VigilWeapons dedicated dual-wire expand residual 2900
//           (prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660 / 3705 / 3750; formula unchanged: currentFloor == Floor100)
//
// Dual-wire index:
//   - 2900: ShouldDropFloor100VigilWeapons residual pure dual-wire
//   - 2902: CanClaimRuneHandler residual pure dual-wire
//   - 3240: CanClaimRuneHandler prior dedicated dual-wire expand residual 2902
//           (retained)
//   - 3281: CanClaimRuneHandler prior dedicated dual-wire expand residual 2902
//           (retained; prior dedicated 3240)
//   - 3311: CanClaimRuneHandler = runeHandler == 0
//     dedicated dual-wire expand residual 2902 (prior dedicated 3281)
//   - 3352: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained)
//   - 3449: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352)
//   - 3502: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449)
//   - 3570: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502)
//   - 3615: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570)
//   - 3660: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615)
//   - 3705: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660)
//   - 3750: ShouldDropFloor100VigilWeapons prior dedicated dual-wire expand residual 2900
//           (retained; prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660 / 3705)
//   - 3795: ShouldDropFloor100VigilWeapons = currentFloor == Floor100
//     dedicated dual-wire expand residual 2900 (prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660 / 3705 / 3750)
//
// Production hosts are Lua under
// scripts/zones/Nyzul_Isle/instances/nyzul_isle_investigation.lua
// local pickSetPoint, scripts/globals/nyzul.lua xi.nyzul.vigilWeaponDrop /
// xi.nyzul.spawnChest / xi.nyzul.activateRuneOfTransfer /
// xi.nyzul.clearChests, scripts/globals/nyzul/pathos.lua
// xi.nyzul.onGearEngage, and
// scripts/zones/Nyzul_Isle/npcs/Rune_of_Transfer.lua onEventUpdate:
//
//   elseif math.random(1, 30) == 1 and instance:getLocalVar('freeFloor') == 0 then
//     instance:setStage(xi.nyzul.objective.FREE_FLOOR)
//     instance:setLocalVar('freeFloor', 1)
//     -- timer(9000) → setProgress(15)
//   ...
//   if math.random(1, 30) <= 5 then
//     instance:setLocalVar('gearObjective',
//       math.random(xi.nyzul.gearObjective.AVOID_AGRO,
//                   xi.nyzul.gearObjective.DO_NOT_DESTROY))
//   end
//
//   if instance:getLocalVar('Nyzul_Current_Floor') == 100 then
//     -- disk-holder job weapon + random weapon
//   elseif math.random(1, 100) <= 20 and ENABLE_VIGIL_DROPS then
//     -- single random NM vigil weapon
//   end
//
//   if
//       instance and
//       csid == 201 and
//       option ~= utils.EVENT_CANCELLED_OPTION and
//       instance:getLocalVar('runeHandler') == 0
//   then
//     instance:setLocalVar('runeHandler', player:getID())
//     -- release other in-event chars; clear Register localVars
//   end
//
//   elseif mobID < BOSS_OFFSET and ENABLE_NYZUL_CASKETS then
//     if math.random(1, 100) <= 6 then
//       -- find DISAPPEAR casket slot, setPos / NORMAL / animationSub
//     end
//   end
//
//   for runeID = RUNE_OF_TRANSFER_OFFSET, RUNE_OF_TRANSFER_OFFSET + 1 do
//     if GetNPCByID(runeID, instance):getStatus() == xi.status.NORMAL then
//       GetNPCByID(runeID, instance):setAnimationSub(1)
//       break
//     end
//   end
//
//   for cofferID = TREASURE_COFFER_OFFSET, TREASURE_COFFER_OFFSET + 2 do
//     local coffer = GetNPCByID(cofferID, instance)
//     if coffer and coffer:getStatus() ~= xi.status.DISAPPEAR then
//       coffer:setStatus(xi.status.DISAPPEAR)
//       coffer:setAnimationSub(0)
//       coffer:resetLocalVars()
//     end
//   end
//   -- same present + not-DISAPPEAR gate for caskets when ENABLE_NYZUL_CASKETS
//
//   if
//       instance:getLocalVar('gearObjective') == xi.nyzul.gearObjective.AVOID_AGRO and
//       mob:getCE(target) == 0 and
//       mob:getVE(target) == 0 and
//       mob:getLocalVar('initialAgro') == 0
//   then
//     mob:setLocalVar('initialAgro', 1)
//     addGearPenalty(mob)
//   end
//
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining roll/localVar/floor/settings/status/
// CE/VE/objective comparisons. Helpers take host-injected scalars only (no
// instance / entity / NPC pointers). Side effects (setStage FREE_FLOOR,
// freeFloor / gearObjective / runeHandler / initialAgro localVar, Rune of
// Transfer timer / setProgress, gear objective type pick, treasure grants,
// casket NPC activate, release of other in-event chars, setAnimationSub on
// NORMAL rune, clearChests setStatus/setAnimationSub/resetLocalVars,
// addGearPenalty entity apply) remain host-owned.
// Prior pure port: OmegaXI slice 1088 (internal/nyzul floorflow / drops /
// progress).

namespace nyzulhelpers
{

// ---------------------------------------------------------------------------
// Slice 3095 — pickSetPoint free-floor selection gate
// (prior dual-wire expansion: slice 2874; residual pure port: 1088)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3095):
//   Formula unchanged from residual 1088 / prior 2874 dual-wire:
//     ShouldGrantFreeFloor(roll1to30, freeFloorVar) =
//       freeFloorVar == 0 && roll1to30 == FreeFloorRollHit
//   Go dual-wire: nyzul.ShouldGrantFreeFloor (internal/nyzul/free_floor.go).
//   Production host is Lua pickSetPoint (no map-server C++ call site);
//   future Lua host injects roll + freeFloor localVar instead of re-inlining
//   `math.random(1, 30) == 1 and freeFloor == 0`.
//   Host still owns setStage FREE_FLOOR, freeFloor writeback, and the
//   9000 ms setProgress(15) timer. Sibling clear-chest dual-wire (3061)
//   is a separate surface — leave alone.

// FreeFloorRollHit is the free-floor success value for math.random(1, 30)
// (== 1 → ~3.33%).
inline constexpr int32 FreeFloorRollHit = 1;

// ShouldGrantFreeFloor mirrors pickSetPoint free-floor selection:
//   math.random(1, 30) == 1 and freeFloor == 0
//
// Formula (slice 3095 dual-wire; unchanged):
//   ShouldGrantFreeFloor(roll1to30, freeFloorVar) =
//     freeFloorVar == 0 && roll1to30 == FreeFloorRollHit
//
// roll1to30 is the host-injected math.random(1, 30) result.
// freeFloorVar is instance:getLocalVar('freeFloor') (0 = never granted this
// run). Host still owns setStage / freeFloor writeback / timer.
// Dual-wire of Go nyzul.ShouldGrantFreeFloor (free_floor.go / slice 3095).
inline auto ShouldGrantFreeFloor(const int32 roll1to30, const int32 freeFloorVar) -> bool
{
    return freeFloorVar == 0 && roll1to30 == FreeFloorRollHit;
}

// ---------------------------------------------------------------------------
// Slice 2891 — pickSetPoint gear-objective chance gate
// ---------------------------------------------------------------------------

// GearObjectiveRollThreshold is the gear success ceiling for math.random(1, 30)
// (roll ≤ 5 → ~16.7%).
inline constexpr int32 GearObjectiveRollThreshold = 5;

// ShouldRollGearObjective mirrors pickSetPoint gear-objective chance:
//   math.random(1, 30) <= 5
// Implemented as roll >= 1 && roll <= threshold so out-of-range rolls do not
// spuriously succeed. roll1to30 is the host-injected math.random(1, 30)
// result. Host still owns gearObjective localVar writeback and the
// AVOID_AGRO..DO_NOT_DESTROY pick. Boss / free-floor branches short-circuit
// before this gate in Lua; host still owns that branch order.
inline auto ShouldRollGearObjective(const int32 roll1to30) -> bool
{
    return roll1to30 >= 1 && roll1to30 <= GearObjectiveRollThreshold;
}

// ---------------------------------------------------------------------------
// Slice 2902 residual / 3240+3281 prior dedicated / 3311 expand residual 2902
// — Rune of Transfer first-claimer gate (onEventUpdate)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3311):
//   Formula unchanged from residual 1088 / residual dual-wire 2902 /
//   prior dedicated 3240 / 3281:
//     CanClaimRuneHandler(runeHandler) = runeHandler == 0
//   Go dual-wire: nyzul.CanClaimRuneHandler (internal/nyzul/claim_rune.go).
//   Production host is Lua Rune_of_Transfer.lua onEventUpdate (no map-server
//   C++ call site); future Lua host injects runeHandler localVar instead of
//   re-inlining `instance:getLocalVar('runeHandler') == 0`.
//   Host still owns csid/option gates, setLocalVar writeback to player ID,
//   and release of other in-event chars.
//   Sibling IsRuneHandler (event finish / pickSetPoint identity) stays
//   residual 1088 — leave alone.
// Coverage: test_nyzul_claim_rune_3311 (dedicated expand residual 2902;
// not in CMake/main); residual 2902 and prior dedicated 3240 / 3281 suites
// retained.

// CanClaimRuneHandler mirrors Rune_of_Transfer.lua onEventUpdate first claim:
//   instance:getLocalVar('runeHandler') == 0
//
// Formula (slice 3311 dual-wire expand residual 2902; prior dedicated 3281;
// unchanged):
//   CanClaimRuneHandler(runeHandler) = runeHandler == 0
//
// runeHandler is the host-injected localVar (0 = no claimer yet). Host still
// owns csid/option gates, setLocalVar writeback to player ID, and release of
// other in-event chars. Sibling IsRuneHandler (event finish identity) is not
// dual-wired here.
// Dual-wire of Go nyzul.CanClaimRuneHandler (claim_rune.go / slice 3311).
inline auto CanClaimRuneHandler(const int32 runeHandler) -> bool
{
    return runeHandler == 0;
}

// ---------------------------------------------------------------------------
// Slice 2900 residual / 3352+3449+3502+3570+3615+3660+3705+3750 prior dedicated / 3795 expand residual 2900
// — vigilWeaponDrop floor-100 guaranteed-drop gate
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3795):
//   Formula unchanged from residual 1088 / residual dual-wire 2900 /
//   prior dedicated 3352 / 3449 / 3502 / 3570 / 3615 / 3660 / 3705 / 3750:
//     ShouldDropFloor100VigilWeapons(currentFloor) = currentFloor == Floor100
//     // Floor100 == 100
//   Go dual-wire: nyzul.ShouldDropFloor100VigilWeapons
//   (internal/nyzul/floor100_vigil.go).
//   Production host is Lua xi.nyzul.vigilWeaponDrop (no map-server C++ call
//   site); future Lua host injects Nyzul_Current_Floor localVar instead of
//   re-inlining `instance:getLocalVar('Nyzul_Current_Floor') == 100`.
//   Host still owns disk-holder / random treasure grants on true and the
//   non-100 NM 20% roll path on false (ShouldRollNMVigilWeapon / slice 2909).
//   Sibling free_floor / claim_rune dual-wires left alone — do not thrash.
// Coverage: test_nyzul_floor100_vigil_3795 (dedicated expand residual 2900;
// not in CMake/main); residual 2900 and prior dedicated 3352 / 3449 / 3502 /
// 3570 / 3615 / 3660 / 3705 / 3750 suites retained.

// Floor100 is the floor pin for guaranteed vigil weapon drops
// (Nyzul_Current_Floor == 100).
inline constexpr int32 Floor100 = 100;

// ShouldDropFloor100VigilWeapons mirrors vigilWeaponDrop floor-100 gate:
//   instance:getLocalVar('Nyzul_Current_Floor') == 100
//
// Formula (slice 3795 dual-wire expand residual 2900; prior dedicated 3352 /
// 3449 / 3502 / 3570 / 3615 / 3660 / 3705 / 3750; unchanged):
//   ShouldDropFloor100VigilWeapons(currentFloor) = currentFloor == Floor100
//
// currentFloor is the host-injected Nyzul_Current_Floor localVar. Host still
// owns disk-holder / random treasure grants on true and the non-100 NM 20%
// roll path on false (ShouldRollNMVigilWeapon / slice 2909).
// Dual-wire of Go nyzul.ShouldDropFloor100VigilWeapons
// (floor100_vigil.go / slice 3795).
// No C++ pin redefs — body identical to residual 2900 / prior dedicated pins.
inline auto ShouldDropFloor100VigilWeapons(const int32 currentFloor) -> bool
{
    return currentFloor == Floor100;
}

// ---------------------------------------------------------------------------
// Slice 2909 — vigilWeaponDrop non-floor-100 NM 20% roll
// ---------------------------------------------------------------------------

// VigilNMDropChancePercent is the non-floor-100 NM vigil drop rate for
// math.random(1, 100) (roll ≤ 20 → 20%).
inline constexpr int32 VigilNMDropChancePercent = 20;

// ShouldRollNMVigilWeapon mirrors vigilWeaponDrop non-100 NM path:
//   math.random(1, 100) <= 20 and ENABLE_VIGIL_DROPS
// Implemented as enableVigilDrops && roll >= 1 && roll <= VigilNMDropChancePercent
// so out-of-range rolls do not spuriously succeed. roll1to100 is the
// host-injected math.random(1, 100) result; enableVigilDrops injects
// xi.settings.main.ENABLE_VIGIL_DROPS (default true). Host still owns the
// floor-100 short-circuit (ShouldDropFloor100VigilWeapons) and random
// weapon grant on true.
inline auto ShouldRollNMVigilWeapon(const int32 roll1to100, const bool enableVigilDrops) -> bool
{
    return enableVigilDrops && roll1to100 >= 1 && roll1to100 <= VigilNMDropChancePercent;
}

// ---------------------------------------------------------------------------
// Slice 3128 — spawnChest regular-mob casket roll
// (prior dual-wire expansion: slice 2905; residual pure port: 1088)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3128):
//   Formula unchanged from residual 1088 / prior 2905 dual-wire:
//     ShouldSpawnCasket(roll1to100, enableCaskets) =
//       enableCaskets && roll1to100 >= 1 && roll1to100 <= CasketDropChancePercent
//   Go dual-wire: nyzul.ShouldSpawnCasket (internal/nyzul/spawn_casket.go).
//   Production host is Lua xi.nyzul.spawnChest (no map-server C++ call site);
//   future Lua host injects roll + ENABLE_NYZUL_CASKETS instead of re-inlining
//   `ENABLE_NYZUL_CASKETS and math.random(1, 100) <= 6`.
//   Host still owns mob-band eligibility (mobID < BOSS_OFFSET, NM coffer
//   short-circuit) and casket NPC activate (setPos / NORMAL / animationSub).
//   Sibling clear-chest dual-wire (3061), free-floor dual-wire (3095), and
//   activate-rune dual-wire (3110) are separate surfaces — leave alone.

// CasketDropChancePercent is the regular-mob casket pop rate for
// math.random(1, 100) (roll ≤ 6 → 6%).
inline constexpr int32 CasketDropChancePercent = 6;

// ShouldSpawnCasket mirrors spawnChest regular-mob casket roll:
//   ENABLE_NYZUL_CASKETS and math.random(1, 100) <= 6
// Implemented as enableCaskets && roll >= 1 && roll <= CasketDropChancePercent
// so out-of-range rolls do not spuriously succeed. roll1to100 is the
// host-injected math.random(1, 100) result; enableCaskets injects
// xi.settings.main.ENABLE_NYZUL_CASKETS (default true). Host still owns
// mob-band eligibility (mobID < BOSS_OFFSET, NM coffer short-circuit) and
// casket NPC activate (setPos / NORMAL / animationSub).
//
// Formula (slice 3128 dual-wire; unchanged):
//   ShouldSpawnCasket(roll1to100, enableCaskets) =
//     enableCaskets && roll1to100 >= 1 && roll1to100 <= CasketDropChancePercent
// Dual-wire of Go nyzul.ShouldSpawnCasket (spawn_casket.go / slice 3128).
inline auto ShouldSpawnCasket(const int32 roll1to100, const bool enableCaskets) -> bool
{
    return enableCaskets && roll1to100 >= 1 && roll1to100 <= CasketDropChancePercent;
}

// ---------------------------------------------------------------------------
// Slice 3110 — activateRuneOfTransfer NORMAL status gate
// (prior dual-wire expansion: slice 2913; residual pure port: 1088)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3110):
//   Formula unchanged from residual 1088 / prior 2913 dual-wire:
//     ShouldActivateRuneOfTransfer(status) = status == kStatusNormal
//   Go dual-wire: nyzul.ShouldActivateRuneOfTransfer
//   (internal/nyzul/activate_rune.go).
//   Production host is Lua xi.nyzul.activateRuneOfTransfer (no map-server
//   C++ call site); future Lua host injects status instead of re-inlining
//   `getStatus() == xi.status.NORMAL`.
//   Host still owns the Rune of Transfer offset loop (OFFSET .. OFFSET+1),
//   setAnimationSub(1), and break after the first NORMAL rune.
//   Sibling clear-chest dual-wire (3061) and free-floor dual-wire (3095)
//   are separate surfaces — leave alone.

// Status pins used by activateRuneOfTransfer (and residual clearChests /
// spawnChest gates). Match Go nyzul.StatusNormal / StatusDisappear and
// xi.status.NORMAL / xi.status.DISAPPEAR.
inline constexpr uint8 kStatusNormal    = 0;
inline constexpr uint8 kStatusDisappear = 2;

// ShouldActivateRuneOfTransfer mirrors activateRuneOfTransfer status gate:
//   GetNPCByID(runeID, instance):getStatus() == xi.status.NORMAL
//   ≡ status == kStatusNormal
//
// Formula (slice 3110 dual-wire; unchanged):
//   ShouldActivateRuneOfTransfer(status) = status == kStatusNormal
//
// status is the host-injected NPC status scalar. Host still owns the Rune of
// Transfer offset loop (OFFSET .. OFFSET+1), setAnimationSub(1), and break
// after the first NORMAL rune.
// Dual-wire of Go nyzul.ShouldActivateRuneOfTransfer (activate_rune.go / slice 3110).
inline auto ShouldActivateRuneOfTransfer(const uint8 status) -> bool
{
    return status == kStatusNormal;
}

// ---------------------------------------------------------------------------
// Slice 3061 — clearChests present + not-DISAPPEAR status gate
// (prior dual-wire expansion: slice 2914; residual pure port: 1088)
// ---------------------------------------------------------------------------
// Dual-wire notes (slice 3061):
//   Formula unchanged from residual 1088 / prior 2914 dual-wire:
//     ShouldClearChestNPC(present, status) = present && status != kStatusDisappear
//   Go dual-wire: nyzul.ShouldClearChestNPC (internal/nyzul/clear_chest.go).
//   Production host is Lua xi.nyzul.clearChests (no map-server C++ call site);
//   future Lua host injects present + status instead of re-inlining
//   `coffer and coffer:getStatus() ~= xi.status.DISAPPEAR`.
//   Host still owns coffer/casket offset loops, ENABLE_NYZUL_CASKETS gating,
//   and setStatus(DISAPPEAR) / setAnimationSub(0) / resetLocalVars side effects.
//   Status pin constants: kStatusNormal / kStatusDisappear (shared with 2913).

// ShouldClearChestNPC mirrors clearChests status gate before DISAPPEAR reset:
//   npc and npc:getStatus() ~= xi.status.DISAPPEAR
//   ≡ present && status != kStatusDisappear
//
// Formula (slice 3061 dual-wire; unchanged):
//   ShouldClearChestNPC(present, status) = present && status != kStatusDisappear
//
// present is whether GetNPCByID returned a non-nil entity; status is the
// host-injected NPC status scalar. Host still owns coffer/casket offset
// loops, ENABLE_NYZUL_CASKETS gating, and setStatus(DISAPPEAR) /
// setAnimationSub(0) / resetLocalVars side effects.
// Dual-wire of Go nyzul.ShouldClearChestNPC (clear_chest.go / slice 3061).
inline auto ShouldClearChestNPC(const bool present, const uint8 status) -> bool
{
    return present && status != kStatusDisappear;
}

// ---------------------------------------------------------------------------
// Slice 2918 — onGearEngage AVOID_AGRO penalty-trigger gate
// ---------------------------------------------------------------------------

// GearObjectiveAvoidAgro is xi.nyzul.gearObjective.AVOID_AGRO (pathos.lua /
// nyzul.lua catalog pin). Match Go nyzul.GearObjectiveAvoidAgro.
inline constexpr int32 GearObjectiveAvoidAgro = 1;

// ShouldApplyGearEngagePenalty mirrors onGearEngage penalty-trigger gate:
//   gearObjective == AVOID_AGRO and CE == 0 and VE == 0 and initialAgro == 0
// gearObjective is the host-injected instance gearObjective localVar.
// ce / ve are host-injected mob:getCE(target) / getVE(target).
// initialAgro is mob:getLocalVar('initialAgro') (0 = first clean engage).
// Host still sets initialAgro = 1 and calls addGearPenalty. Sibling
// onGearDeath / DO_NOT_DESTROY residual gate is not dual-wired here.
inline auto ShouldApplyGearEngagePenalty(const int32 gearObjective, const int32 ce, const int32 ve, const int32 initialAgro) -> bool
{
    return gearObjective == GearObjectiveAvoidAgro &&
           ce == 0 &&
           ve == 0 &&
           initialAgro == 0;
}

} // namespace nyzulhelpers
