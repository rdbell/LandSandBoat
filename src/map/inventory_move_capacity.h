#pragma once

#include <cstdint>

// Pure inventory scan/move/recycle policy from charutils.

namespace inventorymovehelpers
{

// Container pins from item_container.h.
constexpr std::uint8_t MaxContainerID  = 18;
constexpr std::uint8_t ErrorSlotID     = 255;
constexpr std::uint8_t LocRecycleBin   = 17;
constexpr std::uint8_t RecycleBinSize  = 10; // slots 1..10
constexpr std::uint8_t RecycleBinFirst = 1;
constexpr std::uint8_t RecycleBinLast  = 10;

// ShouldRejectZeroItemID mirrors ItemID == 0 for HasItem/getItemCount.
constexpr auto ShouldRejectZeroItemID(const std::uint16_t itemID) -> bool
{
    return itemID == 0;
}

// IsContainerLoopID mirrors LocID < MAX_CONTAINER_ID.
constexpr auto IsContainerLoopID(const std::uint8_t locID) -> bool
{
    return locID < MaxContainerID;
}

// FoundInStorage mirrors SearchItem != ERROR_SLOTID.
constexpr auto FoundInStorage(const std::uint8_t searchSlot) -> bool
{
    return searchSlot != ErrorSlotID;
}

// AccumulateItemCount mirrors itemCount += quantity when IDs match.
constexpr auto AccumulateItemCount(const std::uint32_t itemCount, const std::uint16_t wantID, const std::uint16_t haveID, const std::uint32_t quantity) -> std::uint32_t
{
    if (wantID == haveID)
    {
        return itemCount + quantity;
    }
    return itemCount;
}

// --- MoveItem pure gates ---

// ShouldRejectMoveNoFreeSlots mirrors freeSlots == 0.
constexpr auto ShouldRejectMoveNoFreeSlots(const std::uint8_t freeSlots) -> bool
{
    return freeSlots == 0;
}

// ShouldRejectMoveTargetOccupied mirrors NewSlotID != ERROR && GetItem(NewSlot) != null.
constexpr auto ShouldRejectMoveTargetOccupied(const std::uint8_t newSlotID, const bool targetOccupied) -> bool
{
    return newSlotID != ErrorSlotID && targetOccupied;
}

// ShouldRejectMoveRemoveFailed mirrors RemoveItem returned null.
constexpr auto ShouldRejectMoveRemoveFailed(const bool removed) -> bool
{
    return !removed;
}

// ShouldAutoAssignMoveSlot mirrors NewSlotID == ERROR_SLOTID (auto insert).
constexpr auto ShouldAutoAssignMoveSlot(const std::uint8_t newSlotID) -> bool
{
    return newSlotID == ErrorSlotID;
}

// ShouldRejectMoveInsertFailed mirrors InsertItem returned ERROR_SLOTID.
constexpr auto ShouldRejectMoveInsertFailed(const std::uint8_t insertedSlot) -> bool
{
    return insertedSlot == ErrorSlotID;
}

// ShouldRollbackMoveDBFailure mirrors !rset || !rowsAffected.
constexpr auto ShouldRollbackMoveDBFailure(const bool dbOK, const bool rowsAffected) -> bool
{
    return !dbOK || !rowsAffected;
}

// --- DropItem ---

// ShouldEmitDropMessages mirrors UpdateItem return != 0.
constexpr auto ShouldEmitDropMessages(const std::uint32_t updateItemResult) -> bool
{
    return updateItemResult != 0;
}

// DropQuantityDelta is -quantity for UpdateItem.
constexpr auto DropQuantityDelta(const std::int32_t quantity) -> std::int32_t
{
    return -quantity;
}

// --- Recycle bin ---

// ShouldRejectRecycleNullSource mirrors PSrcItem == nullptr.
constexpr auto ShouldRejectRecycleNullSource(const bool hasSrc) -> bool
{
    return !hasSrc;
}

// ShouldRejectRecycleBusy mirrors isBusy.
constexpr auto ShouldRejectRecycleBusy(const bool isBusy) -> bool
{
    return isBusy;
}

// ShouldUseSimpleRecycleMove mirrors freeSlots > 0.
constexpr auto ShouldUseSimpleRecycleMove(const std::uint8_t recycleFreeSlots) -> bool
{
    return recycleFreeSlots > 0;
}

// ShouldRejectRecycleMoveFailed mirrors MoveItemTo returned ERROR_SLOTID.
constexpr auto ShouldRejectRecycleMoveFailed(const std::uint8_t newSlotID) -> bool
{
    return newSlotID == ErrorSlotID;
}

// ShouldRollbackRecycleDBFailure mirrors !rset || !rowsAffected.
constexpr auto ShouldRollbackRecycleDBFailure(const bool dbOK, const bool rowsAffected) -> bool
{
    return !dbOK || !rowsAffected;
}

// RecycleBinEvictSlot is always slot 1 when bin full.
constexpr std::uint8_t RecycleBinEvictSlot = 1;

// RecycleBinSlideRange: slots 2..10 slide to 1..9.
constexpr auto IsRecycleBinSlideSourceSlot(const std::int32_t slot) -> bool
{
    return slot >= 2 && slot <= 10;
}

// RecycleBinSlideTarget is slot - 1.
constexpr auto RecycleBinSlideTarget(const std::int32_t slot) -> std::int32_t
{
    return slot - 1;
}

// ShouldSkipEmptySlideSlot mirrors GetItem(i) == nullptr.
constexpr auto ShouldSkipEmptySlideSlot(const bool hasItem) -> bool
{
    return !hasItem;
}

// RecycleBinInsertSlot when full after slide is always 10.
constexpr std::uint8_t RecycleBinFullInsertSlot = 10;

// EmptyRecycleBin iterates slots 1..size inclusive.
constexpr auto IsEmptyRecycleBinSlot(const std::uint8_t slotID, const std::uint8_t binSize) -> bool
{
    return slotID >= 1 && slotID <= binSize;
}

// RecycleBinLocationForDelete is 17 (LOC_RECYCLEBIN).
constexpr std::uint8_t RecycleBinLocationForDelete = LocRecycleBin;

} // namespace inventorymovehelpers
