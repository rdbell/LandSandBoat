#pragma once

#include "common/cbasetypes.h"

// Pure Salvage helpers shared by dual-wire slices:
//   - 2871: CanClaimTransport / TransportUserBusy (onTransportUpdate gate)
//
// Lua production host: scripts/globals/salvage.lua xi.salvage.onTransportUpdate:
//
//   if instance:getLocalVar('transportUser') == 0 then
//     -- claim path: set transportUser, stageComplete=0, resetTempBoxes, ...
//   else
//     return
//   end
//
// Host injects the transportUser local-var scalar only (no instance pointer).
// Claim writeback (setLocalVar, timer clear, deSpawnStage, release) remains
// host-owned. Future Lua/C++ hosts dual-wire these free functions instead of
// re-inlining the transportUser == 0 comparison.

namespace salvagehelpers
{

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

} // namespace salvagehelpers
