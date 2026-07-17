#pragma once

#include "common/cbasetypes.h"

// Pure Ambuscade helpers shared by dual-wire slices:
//   - 2875: onInstanceTimeUpdate complete-when-no-mobs-alive gate
//   - 2888: instance onEventFinish exit-warp CSID gate
//   - 2895: Gorpa-Masorpa onTrade eminence-completed(499) gate
//
// Production host is Lua under
// scripts/zones/Maquette_Abdhaljs-Legion_B/instances/ambuscade.lua
// (onInstanceTimeUpdate, onEventFinish) and scripts/globals/ambuscade.lua
// (onTradeGorpaMasorpa). Capacity is for future Lua/C++ inject so hosts
// dual-wire pure free functions instead of re-inlining comparisons.
// Helpers take host-injected scalars only (no entity / instance / mob
// pointers). Side effects (instance:complete, currency/KI writeback,
// setPos, trade body) remain host-owned.
//
// Parity: internal/ambuscade complete_instance.go, warp_exit.go,
// gorpa_trade.go

namespace ambuscadehelpers
{

// ---------------------------------------------------------------------------
// Slice 2875 — onInstanceTimeUpdate complete gate
// ---------------------------------------------------------------------------

// ShouldCompleteInstance mirrors ambuscade.lua onInstanceTimeUpdate:
//   if not mobsStillAlive then instance:complete() end
// anyMobAlive is the host inject for the per-mob isAlive loop result
// (mobsStillAlive). Host still calls instance:complete() after a true gate.
inline auto ShouldCompleteInstance(const bool anyMobAlive) -> bool
{
    return !anyMobAlive;
}

// ---------------------------------------------------------------------------
// Slice 2888 — instance onEventFinish exit-warp CSID gate
// ---------------------------------------------------------------------------

// EventCSIDExit is the exit cutscene CSID (ambuscade instance onEventFinish /
// onInstanceComplete / failure startEvent). Parity: Go EventCSIDExit.
inline constexpr int32 EventCSIDExit = 10001;

// ShouldWarpOnExitEvent mirrors ambuscade.lua instance onEventFinish:
//   if csid == 10001 then player:setPos(...) end
// csid is the host-injected event CSID. Host still calls setPos(ExitDest)
// after a true gate.
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

} // namespace ambuscadehelpers
