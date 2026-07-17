#pragma once

#include "common/cbasetypes.h"

// Pure Ambuscade helpers shared by dual-wire slices:
//   - 2875: onInstanceTimeUpdate complete-when-no-mobs residual dual-wire suite
//   - 2888: instance onEventFinish exit-warp CSID residual dual-wire suite
//   - 2895: Gorpa-Masorpa onTrade eminence-completed(499) gate
//   - 2901: Ambuscade Tome onEventFinish enter CSID (378) gate
//   - 2906: Gorpa-Masorpa onEventFinish intro CSID (385) → RoE 499 residual
//   - 2910: Ambuscade Tome onEventFinish Intense VE createInstance residual
//   - 2917: onInstanceComplete / onInstanceFailure always-start exit CS residual
//   - 3062: ShouldCompleteInstance prior dedicated dual-wire (complete_instance.go)
//   - 3088: ShouldWarpOnExitEvent prior dedicated dual-wire (warp_exit.go)
//   - 3109: ShouldStartExitEvent prior dedicated dual-wire (start_exit.go)
//   - 3129: ShouldTriggerRoEIntro dedicated dual-wire (roe_intro.go)
//   - 3143: ShouldCreateIntenseVEInstance dedicated dual-wire (intense_ve.go)
//   - 3241: ShouldCompleteInstance dedicated expand residual 2875 (prior 3062)
//   - 3356: ShouldWarpOnExitEvent dedicated expand residual 2888 (prior 3088)
//   - 3382: ShouldStartExitEvent prior dedicated expand residual 2917 (prior 3109)
//   - 3438: ShouldStartExitEvent dedicated expand residual 2917 (prior 3382/3109)
//
// Dual-wire index:
//   - 2875: ShouldCompleteInstance residual dual-wire suite
//   - 2888: ShouldWarpOnExitEvent residual dual-wire suite
//   - 2895: ShouldProcessGorpaTrade (eminenceCompleted 499)
//   - 2901: ShouldHandleTomeEnterFinish (csid 378)
//   - 2906: ShouldTriggerRoEIntro residual dual-wire suite
//   - 2910: ShouldCreateIntenseVEInstance residual dual-wire suite
//   - 2917: ShouldStartExitEvent residual dual-wire suite
//   - 3062: ShouldCompleteInstance prior dedicated dual-wire (!anyMobAlive)
//   - 3088: ShouldWarpOnExitEvent prior dedicated dual-wire (csid == EventCSIDExit / 10001)
//   - 3109: ShouldStartExitEvent prior dedicated dual-wire (always true / startEvent 10001)
//   - 3129: ShouldTriggerRoEIntro (csid == EventCSIDIntro / 385 → RoE 499)
//   - 3143: ShouldCreateIntenseVEInstance (csid 374 + option 5 → createInstance 30000)
//   - 3241: ShouldCompleteInstance dedicated expand residual 2875 (prior 3062)
//   - 3356: ShouldWarpOnExitEvent dedicated expand residual 2888 (prior 3088)
//   - 3382: ShouldStartExitEvent prior dedicated expand residual 2917 (prior 3109)
//   - 3438: ShouldStartExitEvent dedicated expand residual 2917 (prior 3382/3109)
//
// Production host is Lua under
// scripts/zones/Maquette_Abdhaljs-Legion_B/instances/ambuscade.lua
// (onInstanceTimeUpdate, onEventFinish) and scripts/globals/ambuscade.lua
// (onTradeGorpaMasorpa, onEventFinishTome, onEventFinishGorpaMasorpa,
// onInstanceComplete, onInstanceFailure).
// Capacity is for future Lua/C++ inject so hosts dual-wire pure free
// functions instead of re-inlining comparisons. Helpers take host-injected
// scalars only (no entity / instance / mob pointers). Side effects
// (instance:complete, currency/KI writeback, setPos, trade body, tome
// enter body, RoE onRecordTrigger, createInstance, startEvent) remain
// host-owned.
// Go dual-wire: ambuscade.ShouldCompleteInstance
// (internal/ambuscade/complete_instance.go). Future Lua host injects
// ShouldCompleteInstance then instance:complete().
//
// Parity: internal/ambuscade complete_instance.go, warp_exit.go,
// gorpa_trade.go, tome_enter.go, roe_intro.go, intense_ve.go,
// start_exit.go

namespace ambuscadehelpers
{

// ---------------------------------------------------------------------------
// Slice 2875 / 3062 / 3241 — onInstanceTimeUpdate complete gate
// ---------------------------------------------------------------------------

// ShouldCompleteInstance mirrors ambuscade.lua onInstanceTimeUpdate:
//   if not mobsStillAlive then instance:complete() end
//
// Formula (slice 3241 dedicated dual-wire expand residual 2875; prior
// dedicated 3062 — formula unchanged):
//   ShouldCompleteInstance(anyMobAlive) = !anyMobAlive
//
// anyMobAlive — host inject for the per-mob isAlive OR fold result
//               (Lua local mobsStillAlive)
// true  → host calls instance:complete()
// false → leave instance running
//
// Dual-wire of Go ambuscade.ShouldCompleteInstance.
// Call site: future Lua onInstanceTimeUpdate inject.
// Prior pure port: slice 1089. Residual dual-wire suite: 2875 /
// test_ambuscade_complete_instance_2875. Prior dedicated dual-wire suite:
// test_ambuscade_complete_instance_3062. Dedicated expand residual suite is
// test_ambuscade_complete_instance_3241. Host still calls complete() after
// a true gate.
inline auto ShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

// ---------------------------------------------------------------------------
// Slice 2888 / 3088 / 3356 — instance onEventFinish exit-warp CSID gate
// ---------------------------------------------------------------------------

// EventCSIDExit is the exit cutscene CSID (ambuscade instance onEventFinish /
// onInstanceComplete / failure startEvent). Parity: Go EventCSIDExit.
inline constexpr int32 EventCSIDExit = 10001;

// ShouldWarpOnExitEvent mirrors ambuscade.lua instance onEventFinish:
//   if csid == 10001 then player:setPos(...) end
//
// Formula (slice 3356 dedicated dual-wire expand residual 2888; prior
// dedicated 3088 — formula unchanged):
//   ShouldWarpOnExitEvent(csid) = csid == EventCSIDExit  // 10001
//
// csid — host-injected event CSID from onEventFinish
// true  → host calls setPos(ExitDest) (Mhaura)
// false → no exit warp
//
// Dual-wire of Go ambuscade.ShouldWarpOnExitEvent.
// Call site: future Lua onEventFinish inject.
// Prior pure port: slice 1089. Residual dual-wire suite: 2888 /
// test_ambuscade_warp_exit_2888. Prior dedicated dual-wire suite:
// test_ambuscade_warp_exit_3088. Dedicated expand residual suite is
// test_ambuscade_warp_exit_3356. Host still calls setPos(ExitDest) after
// a true gate. EventCSIDExit is also the complete/failure startEvent CSID;
// this gate is the finish-side warp half only.
inline auto ShouldWarpOnExitEvent(const int32 csid) -> bool
{
    return csid == EventCSIDExit;
}

// ---------------------------------------------------------------------------
// Slice 2895 — Gorpa-Masorpa onTrade eminence-completed gate
// ---------------------------------------------------------------------------

// RoERecordSteppingIntoAnAmbuscade is RoE record #499
// ("Stepping into an Ambuscade"). Parity: Go RoERecordSteppingIntoAnAmbuscade.
inline constexpr int32 RoERecordSteppingIntoAnAmbuscade = 499;

// ShouldProcessGorpaTrade mirrors ambuscade.lua onTradeGorpaMasorpa:
//   if player:getEminenceCompleted(499) then -- TODO end
// eminenceCompleted499 is the host inject for getEminenceCompleted(499).
// Host still owns the (currently empty) trade body after a true gate.
inline auto ShouldProcessGorpaTrade(const bool eminenceCompleted499) -> bool
{
    return eminenceCompleted499;
}

// ---------------------------------------------------------------------------
// Slice 2901 — Ambuscade Tome onEventFinish enter CSID gate
// ---------------------------------------------------------------------------

// EventCSIDTomeEnter is the Ambuscade Tome enter cutscene CSID
// (ambuscade.lua onEventFinishTome / commented startEvent path).
// Parity: Go EventCSIDTomeEnter.
inline constexpr int32 EventCSIDTomeEnter = 378;

// ShouldHandleTomeEnterFinish mirrors ambuscade.lua onEventFinishTome:
//   elseif csid == 378 then -- TODO end
// csid is the host-injected event CSID. Host still owns the (currently
// empty) enter-finish body after a true gate.
inline auto ShouldHandleTomeEnterFinish(const int32 csid) -> bool
{
    return csid == EventCSIDTomeEnter;
}

// ---------------------------------------------------------------------------
// Slice 2906 / 3129 — Gorpa-Masorpa onEventFinish intro RoE CSID gate
// ---------------------------------------------------------------------------

// EventCSIDIntro is the Gorpa-Masorpa intro cutscene CSID
// (ambuscade.lua onEventFinishGorpaMasorpa / onTrigger intro startEvent).
// Parity: Go EventCSIDIntro.
inline constexpr int32 EventCSIDIntro = 385;

// ShouldTriggerRoEIntro mirrors ambuscade.lua onEventFinishGorpaMasorpa:
//   if csid == 385 then xi.roe.onRecordTrigger(player, 499) end
//
// Formula (slice 3129 dual-wire; residual expand 2906):
//   ShouldTriggerRoEIntro(csid) = csid == EventCSIDIntro  // 385
//
// csid — host-injected event CSID from onEventFinishGorpaMasorpa
// true  → host calls roe.onRecordTrigger(player, 499)
// false → no RoE intro trigger
//
// Dual-wire of Go ambuscade.ShouldTriggerRoEIntro.
// Call site: future Lua onEventFinishGorpaMasorpa inject.
// Prior pure port: slice 1005. Residual dual-wire suite: 2906 /
// test_ambuscade_roe_intro_2906. Dedicated dual-wire suite is
// test_ambuscade_roe_intro_3129. Host still calls
// roe.onRecordTrigger(player, RoERecordSteppingIntoAnAmbuscade) after a
// true gate. EventCSIDIntro is also the intro startEvent CSID on Gorpa
// trigger; this gate is the finish-side half only.
inline auto ShouldTriggerRoEIntro(const int32 csid) -> bool
{
    return csid == EventCSIDIntro;
}

// ---------------------------------------------------------------------------
// Slice 2910 / 3143 — Ambuscade Tome onEventFinish Intense VE createInstance gate
// ---------------------------------------------------------------------------

// EventCSIDTomeRegister is the Ambuscade Tome register cutscene CSID
// (ambuscade.lua onEventFinishTome / onEventUpdateTome register path).
// Parity: Go EventCSIDTomeRegister.
inline constexpr int32 EventCSIDTomeRegister = 374;

// TomeOptionIntenseVE is registration option 5 (Intense Very Easy).
// Parity: Go TomeOptionIntenseVE.
inline constexpr int32 TomeOptionIntenseVE = 5;

// InstanceIntenseVE is the createInstance id for Intense VE (option 5).
// Parity: Go InstanceIntenseVE. LSB hard-codes 30000.
inline constexpr int32 InstanceIntenseVE = 30000;

// ShouldCreateIntenseVEInstance mirrors ambuscade.lua onEventFinishTome:
//   if csid == 374 and option == 5 then player:createInstance(30000) end
//
// Formula (slice 3143 dual-wire; residual expand 2910):
//   ShouldCreateIntenseVEInstance(csid, option) =
//     csid == EventCSIDTomeRegister && option == TomeOptionIntenseVE
//   // EventCSIDTomeRegister = 374
//   // TomeOptionIntenseVE   = 5
//
// csid, option — host-injected event scalars from onEventFinishTome
// true  → host calls createInstance(InstanceIntenseVE) (30000)
// false → no hard-coded createInstance
//
// Dual-wire of Go ambuscade.ShouldCreateIntenseVEInstance.
// Call site: future Lua onEventFinishTome inject.
// Prior pure port: slice 1005. Residual dual-wire suite: 2910 /
// test_ambuscade_intense_ve_2910. Dedicated dual-wire suite is
// test_ambuscade_intense_ve_3143. Host still calls
// createInstance(InstanceIntenseVE) after a true gate. Other tome options
// do not take this hard-coded createInstance path upstream.
inline auto ShouldCreateIntenseVEInstance(const int32 csid, const int32 option) -> bool
{
    return csid == EventCSIDTomeRegister && option == TomeOptionIntenseVE;
}

// ---------------------------------------------------------------------------
// Slice 2917 / 3109 / 3382 / 3438 — onInstanceComplete / onInstanceFailure always-start exit CS
// ---------------------------------------------------------------------------

// ShouldStartExitEvent mirrors ambuscade.lua onInstanceComplete and
// onInstanceFailure: both paths always call player:startEvent(10001) for
// every char (no additional gate).
//
// Formula (slice 3438 dedicated dual-wire expand residual 2917; prior
// dedicated 3382/3109 — formula unchanged):
//   ShouldStartExitEvent() = true
//
// true → host calls startEvent(EventCSIDExit) (10001) for every char
//
// Dual-wire of Go ambuscade.ShouldStartExitEvent.
// Call site: future Lua onInstanceComplete / onInstanceFailure inject.
// Prior pure port: slice 1089. Residual dual-wire suite: 2917 /
// test_ambuscade_start_exit_2917. Prior dedicated dual-wire suite:
// test_ambuscade_start_exit_3109. Prior dedicated expand residual suite:
// test_ambuscade_start_exit_3382. Dedicated expand residual suite is
// test_ambuscade_start_exit_3438. Host still calls startEvent(EventCSIDExit)
// after a true gate. Pure surface is unconditional so hosts dual-wire one
// free function instead of re-inlining "always start exit CS".
// Parity: Go ShouldStartExitEvent.
inline auto ShouldStartExitEvent() -> bool
{
    return true;
}

} // namespace ambuscadehelpers
