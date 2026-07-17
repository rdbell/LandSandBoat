#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>

// Pure CTreasurePool add/lot/eviction policy halves.

namespace treasurepoolhelpers
{

// TREASUREPOOL_SIZE mirror.
constexpr uint8 PoolSize = 10;

// FreeSlotID starts as uint8(-1) == 255; "unset" is FreeSlotID > PoolSize.
constexpr uint8 FreeSlotUnset = static_cast<uint8>(-1);

// Pass lot mask FF FF.
constexpr uint16 PassedLot = 65535;

// treasure_checktime mirror (seconds).
constexpr uint32 TreasureCheckTimeSeconds = 3;

// IsFreeSlotUnset mirrors FreeSlotID > TREASUREPOOL_SIZE after uint8(-1) init.
inline auto IsFreeSlotUnset(const uint8 freeSlotID) -> bool
{
    return freeSlotID > PoolSize;
}

// FirstEmptySlotFound when freeSlotID was set from an empty slot (ID==0).
inline auto IsValidFreeSlot(const uint8 freeSlotID) -> bool
{
    return freeSlotID < PoolSize;
}

// ShouldSkipRareCheck mirrors non-solo pool + NoRareCheck item flag.
inline auto ShouldSkipRareCheck(const bool isSoloPool, const bool itemHasNoRareCheck) -> bool
{
    return !isSoloPool && itemHasNoRareCheck;
}

// ShouldApplyRareMemberCheck mirrors rare item and not skip.
inline auto ShouldApplyRareMemberCheck(const bool itemIsRare, const bool skipRareCheck) -> bool
{
    return itemIsRare && !skipRareCheck;
}

// ShouldRejectRareAllHave mirrors everyone already owns the rare item.
inline auto ShouldRejectRareAllHave(const bool applyRareCheck, const bool anyMemberMissingRare) -> bool
{
    return applyRareCheck && !anyMemberMissingRare;
}

// CanEvictNonRareNonExclusive mirrors !Rare && !Exclusive for oldest pass 1.
inline auto CanEvictNonRareNonExclusive(const bool hasRare, const bool hasExclusive) -> bool
{
    return !hasRare && !hasExclusive;
}

// CanEvictNonExclusive mirrors !Exclusive for oldest pass 2.
inline auto CanEvictNonExclusive(const bool hasExclusive) -> bool
{
    return !hasExclusive;
}

// PreferOlderTimestamp mirrors TimeStamp < oldest for eviction candidate.
// Times are host-normalized comparable units.
inline auto PreferOlderTimestamp(const int64 candidateStamp, const int64 oldestStamp) -> bool
{
    return candidateStamp < oldestStamp;
}

// DefaultFallbackSlot mirrors FreeSlotID = 0 when all eviction passes fail.
inline auto DefaultFallbackSlot() -> uint8
{
    return 0;
}

// ShouldForceCheckOnFullPoolInsert mirrors SlotID == 10 after free-slot scan.
// SlotID ends at PoolSize when the free-slot loop completes without break.
inline auto ShouldForceCheckOnFullPoolInsert(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == PoolSize;
}

// ShouldAutoResolveSolo mirrors memberCount() == 1 after insert.
inline auto ShouldAutoResolveSolo(const std::size_t memberCount) -> bool
{
    return memberCount == 1;
}

// IsSlotInRange mirrors SlotID >= TREASUREPOOL_SIZE reject for lot/pass.
inline auto IsSlotOutOfRange(const uint8 slotID) -> bool
{
    return slotID >= PoolSize;
}

// CanLotWithInventory mirrors freeSlots != 0.
inline auto CanLotWithInventory(const uint8 freeSlots) -> bool
{
    return freeSlots != 0;
}

// CanLotRareItem mirrors !(rare && alreadyHas).
inline auto CanLotRareItem(const bool itemIsRare, const bool alreadyHasItem) -> bool
{
    return !(itemIsRare && alreadyHasItem);
}

// IsHigherLot mirrors lotInfo.lot > highestLot.
inline auto IsHigherLot(const uint16 candidateLot, const uint16 highestLot) -> bool
{
    return candidateLot > highestLot;
}

// HigherLotSelection returns candidateLot when it strictly exceeds currentHighestLot,
// otherwise keeps currentHighestLot. Mirrors the highest-lot scan update in lotItem.
inline auto HigherLotSelection(const uint16 currentHighestLot, const uint16 candidateLot) -> uint16
{
    if (IsHigherLot(candidateLot, currentHighestLot))
    {
        return candidateLot;
    }
    return currentHighestLot;
}

// ShouldEvaluateTreasureImmediately mirrors Lotters.size() == memberCount() after lot/pass.
inline auto ShouldEvaluateTreasureImmediately(const std::size_t lotterCount, const std::size_t memberCount) -> bool
{
    return lotterCount == memberCount;
}

// PostLotPlan is the pure post-emplace disposition of CTreasurePool::lotItem after
// LotInfo is recorded. Trophy-solution packet push is always host-side; plan only
// gates immediate checkTreasureItem when every member has lotted.
struct PostLotPlan
{
    bool evaluateImmediately; // Lotters.size() == memberCount()
};

// PlanPostLot composes ShouldEvaluateTreasureImmediately after a lotter is added.
// Host always pushes trophy solution packets; evaluateImmediately alone is pure.
inline auto PlanPostLot(const std::size_t lotterCountAfterAdd, const std::size_t memberCount) -> PostLotPlan
{
    return PostLotPlan{ ShouldEvaluateTreasureImmediately(lotterCountAfterAdd, memberCount) };
}

// IsPassedLot mirrors lot == 65535.
inline auto IsPassedLot(const uint16 lot) -> bool
{
    return lot == PassedLot;
}

// ShouldRejectNullMember mirrors PChar null || pool mismatch.
inline auto ShouldRejectNullMember(const bool charNull, const bool poolMismatch) -> bool
{
    return charNull || poolMismatch;
}

// ShouldRejectNullItem mirrors !PNewItem / !PItem for lot.
inline auto ShouldRejectNullItem(const bool itemNull) -> bool
{
    return itemNull;
}

// LotItemPreflight is the pure early-gate disposition of CTreasurePool::lotItem
// before LotInfo is recorded. Host keeps logging/side effects; plan is pure.
enum class LotItemPreflight : uint8
{
    Proceed = 0,
    RejectMember,
    RejectSlot,
    RejectItem,
    RejectFullInventory,
    RejectRareOwned,
};

// PlanLotItemPreflight short-circuits in production lotItem order:
// 1) null char or pool mismatch
// 2) slot out of range
// 3) null item lookup
// 4) free inventory slots == 0
// 5) rare && already has
// 6) proceed to record lot
// Composes ShouldRejectNullMember / IsSlotOutOfRange host flag / ShouldRejectNullItem /
// CanLotWithInventory / CanLotRareItem.
inline auto PlanLotItemPreflight(
    const bool charNull,
    const bool poolMismatch,
    const bool slotOutOfRange,
    const bool itemNull,
    const uint8 freeSlots,
    const bool itemIsRare,
    const bool alreadyHasItem) -> LotItemPreflight
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return LotItemPreflight::RejectMember;
    }
    if (slotOutOfRange)
    {
        return LotItemPreflight::RejectSlot;
    }
    if (ShouldRejectNullItem(itemNull))
    {
        return LotItemPreflight::RejectItem;
    }
    if (!CanLotWithInventory(freeSlots))
    {
        return LotItemPreflight::RejectFullInventory;
    }
    if (!CanLotRareItem(itemIsRare, alreadyHasItem))
    {
        return LotItemPreflight::RejectRareOwned;
    }
    return LotItemPreflight::Proceed;
}

// PassItemPreflight is the pure early-gate disposition of CTreasurePool::passItem
// before LotInfo is recorded or mutated. Host keeps logging/side effects; plan is pure.
enum class PassItemPreflight : uint8
{
    Proceed = 0,
    RejectMember,
    RejectSlot,
};

// PlanPassItemPreflight short-circuits in production passItem order:
// 1) null char or pool mismatch
// 2) slot out of range
// 3) proceed to record/mutate the pass
// Composes ShouldRejectNullMember / host slotOutOfRange (from IsSlotOutOfRange).
inline auto PlanPassItemPreflight(
    const bool charNull,
    const bool poolMismatch,
    const bool slotOutOfRange) -> PassItemPreflight
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return PassItemPreflight::RejectMember;
    }
    if (slotOutOfRange)
    {
        return PassItemPreflight::RejectSlot;
    }
    return PassItemPreflight::Proceed;
}

// ShouldUpdatePoolForChar mirrors status != DISAPPEAR.
inline auto ShouldUpdatePoolForChar(const bool isDisappear) -> bool
{
    return !isDisappear;
}

// UpdatePoolPlan is the pure visibility disposition of CTreasurePool::UpdatePool
// before host warns or pushes trophy list packets.
struct UpdatePoolPlan
{
    bool reject;          // null char or pool mismatch
    bool pushTrophyLists; // !reject && status != DISAPPEAR
};

// PlanUpdatePool composes ShouldRejectNullMember and ShouldUpdatePoolForChar.
// Host keeps warning and packet push; plan is pure disposition only.
inline auto PlanUpdatePool(
    const bool charNull,
    const bool poolMismatch,
    const bool isDisappear) -> UpdatePoolPlan
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return UpdatePoolPlan{ true, false };
    }
    return UpdatePoolPlan{ false, ShouldUpdatePoolForChar(isDisappear) };
}

// ShouldFlushPool mirrors m_count != 0.
inline auto ShouldFlushPool(const uint8 itemCount) -> bool
{
    return itemCount != 0;
}

// FlushPlan is the pure disposition of CTreasurePool::flush before the host
// advances tick (now + treasure_checktime + 1s) and checkTreasureItem each slot.
struct FlushPlan
{
    bool runChecks; // itemCount != 0
};

// PlanFlush wraps ShouldFlushPool for the flush entry gate.
// Host keeps timer math and checkTreasureItem loop; plan is pure disposition only.
inline auto PlanFlush(const uint8 itemCount) -> FlushPlan
{
    return FlushPlan{ ShouldFlushPool(itemCount) };
}

// SelectEvictionSlot: pure multi-pass selection over host-provided slot metadata.
// Returns freeSlot if already set; else scans passes; else default 0.
// For each slot index 0..count-1, host provides:
//   empty (ID==0), stamp, hasRare, hasExclusive, itemPresent (lookup non-null).
// This helper only evaluates the three eviction passes when freeSlot is unset.
struct EvictionSlotView
{
    bool  empty;
    bool  itemPresent;
    bool  hasRare;
    bool  hasExclusive;
    int64 stamp;
};

inline auto SelectFreeOrEvictionSlot(
    const uint8 freeSlotIfEmpty,
    const EvictionSlotView* slots,
    const uint8 slotCount) -> uint8
{
    if (!IsFreeSlotUnset(freeSlotIfEmpty))
    {
        return freeSlotIfEmpty;
    }
    if (slots == nullptr || slotCount == 0)
    {
        return DefaultFallbackSlot();
    }

    // Pass 1: oldest non-rare non-ex
    uint8 freeSlot = FreeSlotUnset;
    int64 oldest   = 0;
    bool  hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (s.itemPresent && CanEvictNonRareNonExclusive(s.hasRare, s.hasExclusive) &&
            (!hasOld || PreferOlderTimestamp(s.stamp, oldest)))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }

    // Pass 2: oldest non-ex
    freeSlot = FreeSlotUnset;
    hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (s.itemPresent && CanEvictNonExclusive(s.hasExclusive) &&
            (!hasOld || PreferOlderTimestamp(s.stamp, oldest)))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }

    // Pass 3: oldest any
    freeSlot = FreeSlotUnset;
    hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (!hasOld || PreferOlderTimestamp(s.stamp, oldest))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }
    return DefaultFallbackSlot();
}

// FindFirstEmptySlot returns first empty index or FreeSlotUnset.
inline auto FindFirstEmptySlot(const bool* emptyFlags, const uint8 slotCount) -> uint8
{
    if (emptyFlags == nullptr)
    {
        return FreeSlotUnset;
    }
    for (uint8 i = 0; i < slotCount; ++i)
    {
        if (emptyFlags[i])
        {
            return i;
        }
    }
    return FreeSlotUnset;
}

} // namespace treasurepoolhelpers
