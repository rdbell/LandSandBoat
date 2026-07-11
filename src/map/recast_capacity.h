#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CRecastContainer::Load charge accumulation and HasRecast charge gates.

namespace recasthelpers
{

// ShouldUpdateChargeMeta mirrors chargeTime != 0 / maxCharges != 0 on existing entry.
inline auto ShouldUpdateChargeTime(const bool chargeTimeNonzero) -> bool
{
    return chargeTimeNonzero;
}

inline auto ShouldUpdateMaxCharges(const bool maxChargesNonzero) -> bool
{
    return maxChargesNonzero;
}

// IsSimpleRecast mirrors chargeTime == 0 (no charges — full replace).
inline auto IsSimpleRecast(const bool chargeTimeIsZero) -> bool
{
    return chargeTimeIsZero;
}

// ShouldStampOnZeroRecast mirrors RecastTime == 0 before adding charged duration.
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
inline auto ShouldExpireRecast(const int64 nowUnits, const int64 timeStampUnits, const int64 recastTimeUnits) -> bool
{
    return nowUnits >= timeStampUnits + recastTimeUnits;
}

// ShouldEraseOnExpire mirrors magic (and non-ability) erase vs zero-retain.
inline auto ShouldEraseOnExpire(const bool isAbilityType) -> bool
{
    return !isAbilityType;
}

// IsNewRecastEntry mirrors recast == nullptr on Load.
inline auto IsNewRecastEntry(const bool entryMissing) -> bool
{
    return entryMissing;
}

// DurationSecondsFloor mirrors timer::count_seconds for HasRecast request.
// For non-negative durations this is units / 1s; host supplies already-floored seconds.
inline auto RequestSecondsAsCharges(const int64 durationSeconds) -> int64
{
    return durationSeconds;
}

} // namespace recasthelpers
