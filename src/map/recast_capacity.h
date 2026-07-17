#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CRecastContainer::Load charge accumulation and HasRecast charge gates.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1370: charge Load / HasRecast / expire policy suite
//   - 2800: ShouldResetAbilityRecast / IsOneHourSpecialRecast
//   - 2814: ShouldEraseAbilityOnChangeJob
//   - 2827: RecastIDFromLootRecast
//   - 2931: ShouldStampOnZeroRecast residual dual-wire (expanded 3193)
//   - 3052: ShouldExpireRecast (now >= TimeStamp + RecastTime Check gate)
//   - 3070: ShouldEraseOnExpire residual dual-wire (expanded 3255)
//   - 3104: ShouldUpdateChargeTime residual dual-wire (expanded 3360)
//   - 3122: ShouldUpdateMaxCharges (maxCharges != 0 update gate on Load existing)
//   - 3136: IsSimpleRecast (chargeTime == 0 simple full-replace gate on Load)
//   - 3193: ShouldStampOnZeroRecast (RecastTime == 0 stamp gate on charged Load)
//   - 3255: ShouldEraseOnExpire (!isAbility erase vs ability zero-retain)
//   - 3360: ShouldUpdateChargeTime (chargeTime != 0 update gate on Load existing)
//
// Production host: CRecastContainer::Load (recast_container.cpp) injects
// RecastTime == 0s into ShouldStampOnZeroRecast on the charged path (slice 3193).
// Go dual-wire: recast.ShouldStampOnZeroRecast (internal/recast/stamp_zero.go).
// Check host injects type==RECAST_ABILITY into ShouldEraseOnExpire (slice 3255;
// residual dual-wire 3070).
// Go dual-wire: recast.ShouldEraseOnExpire (internal/recast/erase_on_expire.go).
// Load host injects chargeTime != 0s into ShouldUpdateChargeTime (slice 3360;
// residual dual-wire 3104).
// Go dual-wire: recast.ShouldUpdateChargeTime (internal/recast/update_charge_time.go).
// Load host injects maxCharges != 0 into ShouldUpdateMaxCharges (slice 3122).
// Go dual-wire: recast.ShouldUpdateMaxCharges (internal/recast/update_max_charges.go).
// Load host injects recast->chargeTime == 0s into IsSimpleRecast (slice 3136).
// Go dual-wire: recast.IsSimpleRecast (internal/recast/is_simple_recast.go).

namespace recasthelpers
{

// ShouldUpdateChargeTime mirrors chargeTime != 0 on existing entry.
//
// Formula (slice 3360 dual-wire; residual 3104):
//   chargeTimeNonzero
//
// chargeTimeNonzero — host-evaluated chargeTime != 0s
// true  → overwrite recast->chargeTime with the inject
// false → keep existing chargeTime on the entry
//
// Dual-wire of Go recast.ShouldUpdateChargeTime.
// Call site: CRecastContainer::Load on existing entry before maxCharges /
// simple / charged branches.
// Prior pure port: slice 1370. Residual dual-wire: 3104. Siblings 3052/3070/
// 3255 left alone this slice.
inline auto ShouldUpdateChargeTime(const bool chargeTimeNonzero) -> bool
{
    return chargeTimeNonzero;
}

// ShouldUpdateMaxCharges mirrors maxCharges != 0 on existing entry.
//
// Formula (slice 3122 dual-wire):
//   maxChargesNonzero
//
// maxChargesNonzero — host-evaluated maxCharges != 0
// true  → overwrite recast->maxCharges with the inject
// false → keep existing maxCharges on the entry
//
// Dual-wire of Go recast.ShouldUpdateMaxCharges.
// Call site: CRecastContainer::Load on existing entry after chargeTime update,
// before simple / charged branches.
// Prior pure port: slice 1370. Siblings 3052/3070/3104 left alone this slice.
inline auto ShouldUpdateMaxCharges(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// IsSimpleRecast mirrors chargeTime == 0 (no charges — full replace).
//
// Formula (slice 3136 dual-wire):
//   chargeTimeIsZero
//
// chargeTimeIsZero — host-evaluated recast->chargeTime == 0s
// true  → simple path: stamp TimeStamp = now and replace RecastTime with duration
// false → charged path: ShouldStampOnZeroRecast / charge-cap accumulate
//
// Dual-wire of Go recast.IsSimpleRecast.
// Call site: CRecastContainer::Load on existing entry after chargeTime /
// maxCharges updates, before stamp / charged branches.
// Prior pure port: slice 1370. Siblings 3052/3070/3104/3122 left alone this slice.
inline auto IsSimpleRecast(const bool chargeTimeIsZero) -> bool
{
    return chargeTimeIsZero;
}

// ShouldStampOnZeroRecast mirrors RecastTime == 0 before adding charged duration.
//
// Formula (slice 3193 dual-wire; residual 2931):
//   recastTimeIsZero
//
// recastTimeIsZero — host-evaluated RecastTime == 0s
// true  → stamp TimeStamp to timer::now() before adding charged duration
// false → keep TimeStamp and apply charge-cap overflow adjustment path
//
// Dual-wire of Go recast.ShouldStampOnZeroRecast.
// Call site: CRecastContainer::Load charged branch after IsSimpleRecast is false.
// Prior pure port: slice 1370. Residual dual-wire: 2931. Siblings 3052/3070/
// 3104/3122/3136 left alone this slice.
inline auto ShouldStampOnZeroRecast(const bool recastTimeIsZero) -> bool
{
    return recastTimeIsZero;
}

// ChargeCapUnits mirrors chargeTime * maxCharges in host time units.
inline auto ChargeCapUnits(const int64 chargeTimeUnits, const uint8 maxCharges) -> int64
{
    return chargeTimeUnits * static_cast<int64>(maxCharges);
}

// ExceedsChargeCap mirrors RecastTime + duration > chargeTime * maxCharges.
inline auto ExceedsChargeCap(const int64 recastTimeUnits, const int64 durationUnits, const int64 chargeCapUnits) -> bool
{
    return recastTimeUnits + durationUnits > chargeCapUnits;
}

// ChargeCapOverflowDiff mirrors (RecastTime + duration) - chargeCap.
inline auto ChargeCapOverflowDiff(const int64 recastTimeUnits, const int64 durationUnits, const int64 chargeCapUnits) -> int64
{
    return (recastTimeUnits + durationUnits) - chargeCapUnits;
}

// IsMatchingActiveRecast mirrors ID match && RecastTime > 0.
inline auto IsMatchingActiveRecast(const bool idMatches, const bool recastTimePositive) -> bool
{
    return idMatches && recastTimePositive;
}

// HasRecastWhenSimple mirrors chargeTime == 0 path returning true.
inline auto HasRecastWhenSimple(const bool chargeTimeIsZero) -> bool
{
    return chargeTimeIsZero;
}

// RequestExceedsMaxCharges mirrors crypticRecastSeconds > maxCharges.
inline auto RequestExceedsMaxCharges(const int64 requestedSeconds, const uint8 maxCharges) -> bool
{
    return requestedSeconds > static_cast<int64>(maxCharges);
}

// AvailableCharges mirrors currentRecast / chargeTime (integer division).
// currentRecastUnits and chargeTimeUnits must share the same unit scale.
inline auto AvailableCharges(const int64 currentRecastUnits, const int64 chargeTimeUnits) -> uint8
{
    if (chargeTimeUnits <= 0)
    {
        return 0;
    }
    return static_cast<uint8>(currentRecastUnits / chargeTimeUnits);
}

// RemainingChargesAfterRequest mirrors maxCharges - availableCharges - 1.
inline auto RemainingChargesAfterRequest(const uint8 maxCharges, const uint8 availableCharges) -> int64
{
    return static_cast<int64>(maxCharges) - static_cast<int64>(availableCharges) - 1;
}

// HasInsufficientCharges mirrors charges < requestedSeconds.
inline auto HasInsufficientCharges(const int64 remainingCharges, const int64 requestedSeconds) -> bool
{
    return remainingCharges < requestedSeconds;
}

// CurrentRecastRemaining mirrors TimeStamp - now + RecastTime (may be negative).
inline auto CurrentRecastRemaining(const int64 timeStampUnits, const int64 nowUnits, const int64 recastTimeUnits) -> int64
{
    return timeStampUnits - nowUnits + recastTimeUnits;
}

// IsAbilityRecastType mirrors type == RECAST_ABILITY for Del/Check retention.
inline auto IsAbilityRecastType(const bool isAbility) -> bool
{
    return isAbility;
}

// ShouldExpireRecast mirrors now >= TimeStamp + RecastTime.
//
// Formula (slice 3052 dual-wire):
//   nowUnits >= timeStampUnits + recastTimeUnits
//
// nowUnits / timeStampUnits / recastTimeUnits — host-evaluated epoch units
// (timer::now / TimeStamp / RecastTime.count) sharing the same unit scale
// true  → entry expired (Check erases magic or zeros ability RecastTime)
// false → entry still active
//
// Dual-wire of Go recast.ShouldExpireRecast.
// Call site: CRecastContainer::Check expiry gate.
// Sibling dual-wire: ShouldEraseOnExpire (slice 3255; residual 3070).
inline auto ShouldExpireRecast(const int64 nowUnits, const int64 timeStampUnits, const int64 recastTimeUnits) -> bool
{
    return nowUnits >= timeStampUnits + recastTimeUnits;
}

// ShouldEraseOnExpire mirrors magic (and non-ability) erase vs zero-retain.
//
// Formula (slice 3255 dual-wire; residual 3070):
//   !isAbilityType
//
// isAbilityType — host-evaluated type == RECAST_ABILITY
// true  → retain entry with zero RecastTime (ability)
// false → erase entry (magic / item / loot)
//
// Dual-wire of Go recast.ShouldEraseOnExpire.
// Call site: CRecastContainer::Check after ShouldExpireRecast is true.
// Prior pure port: slice 1370. Residual dual-wire: 3070. Sibling dual-wire:
// ShouldExpireRecast (3052).
inline auto ShouldEraseOnExpire(const bool isAbilityType) -> bool
{
    return !isAbilityType;
}

// IsNewRecastEntry mirrors recast == nullptr on Load.
inline auto IsNewRecastEntry(const bool entryMissing) -> bool
{
    return entryMissing;
}

// RecastIDFromLootRecast mirrors static_cast<Recast>(LootRecastID) for loot
// thin wrappers (HasLootRecast / AddLootRecast). Identity cast of uint16
// (slice 2827). Host still owns Has(RECAST_LOOT, id) lookup.
inline auto RecastIDFromLootRecast(const uint16 lootRecastID) -> uint16
{
    return lootRecastID;
}

// DurationSecondsFloor mirrors timer::count_seconds for HasRecast request.
// For non-negative durations this is units / 1s; host supplies already-floored seconds.
inline auto RequestSecondsAsCharges(const int64 durationSeconds) -> int64
{
    return durationSeconds;
}

// IsOneHourSpecialRecast mirrors Recast::Special || Recast::Special2 (1HR IDs).
inline auto IsOneHourSpecialRecast(const bool isSpecial, const bool isSpecial2) -> bool
{
    return isSpecial || isSpecial2;
}

// ShouldResetAbilityRecast mirrors ResetAbilities skip of one-hours.
// Returns true when Load(0s) should run (ID is not Special and not Special2).
inline auto ShouldResetAbilityRecast(const bool isSpecial, const bool isSpecial2) -> bool
{
    return !IsOneHourSpecialRecast(isSpecial, isSpecial2);
}

// ShouldEraseAbilityOnChangeJob mirrors CCharRecastContainer::ChangeJob erase_if
// predicate: erase when ID is not Special and not Special2 (non-one-hours).
// Same truth table as ShouldResetAbilityRecast / !IsOneHourSpecialRecast.
inline auto ShouldEraseAbilityOnChangeJob(const bool isSpecial, const bool isSpecial2) -> bool
{
    return !IsOneHourSpecialRecast(isSpecial, isSpecial2);
}

} // namespace recasthelpers
