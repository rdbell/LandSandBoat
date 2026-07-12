#include "test_inventory_move_1519.h"

#include "map/inventory_move_capacity.h"

#include <iostream>

namespace
{
using inventorymovehelpers::AccumulateItemCount;
using inventorymovehelpers::DropQuantityDelta;
using inventorymovehelpers::FoundInStorage;
using inventorymovehelpers::IsContainerLoopID;
using inventorymovehelpers::IsEmptyRecycleBinSlot;
using inventorymovehelpers::IsRecycleBinSlideSourceSlot;
using inventorymovehelpers::RecycleBinFullInsertSlot;
using inventorymovehelpers::RecycleBinSlideTarget;
using inventorymovehelpers::ShouldAutoAssignMoveSlot;
using inventorymovehelpers::ShouldEmitDropMessages;
using inventorymovehelpers::ShouldRejectMoveInsertFailed;
using inventorymovehelpers::ShouldRejectMoveNoFreeSlots;
using inventorymovehelpers::ShouldRejectMoveRemoveFailed;
using inventorymovehelpers::ShouldRejectMoveTargetOccupied;
using inventorymovehelpers::ShouldRejectRecycleBusy;
using inventorymovehelpers::ShouldRejectRecycleMoveFailed;
using inventorymovehelpers::ShouldRejectRecycleNullSource;
using inventorymovehelpers::ShouldRejectZeroItemID;
using inventorymovehelpers::ShouldRollbackMoveDBFailure;
using inventorymovehelpers::ShouldRollbackRecycleDBFailure;
using inventorymovehelpers::ShouldSkipEmptySlideSlot;
using inventorymovehelpers::ShouldUseSimpleRecycleMove;

auto Check() -> bool
{
    if (!ShouldRejectZeroItemID(0) || ShouldRejectZeroItemID(1))
    {
        return false;
    }
    if (!IsContainerLoopID(0) || !IsContainerLoopID(17) || IsContainerLoopID(18))
    {
        return false;
    }
    if (!FoundInStorage(0) || !FoundInStorage(10) || FoundInStorage(255))
    {
        return false;
    }
    if (AccumulateItemCount(3, 100, 100, 5) != 8 || AccumulateItemCount(3, 100, 101, 5) != 3)
    {
        return false;
    }
    if (!ShouldRejectMoveNoFreeSlots(0) || ShouldRejectMoveNoFreeSlots(1))
    {
        return false;
    }
    if (!ShouldRejectMoveTargetOccupied(5, true) || ShouldRejectMoveTargetOccupied(255, true) || ShouldRejectMoveTargetOccupied(5, false))
    {
        return false;
    }
    if (!ShouldRejectMoveRemoveFailed(false) || ShouldRejectMoveRemoveFailed(true))
    {
        return false;
    }
    if (!ShouldAutoAssignMoveSlot(255) || ShouldAutoAssignMoveSlot(3))
    {
        return false;
    }
    if (!ShouldRejectMoveInsertFailed(255) || ShouldRejectMoveInsertFailed(2))
    {
        return false;
    }
    if (!ShouldRollbackMoveDBFailure(false, true) || !ShouldRollbackMoveDBFailure(true, false) || ShouldRollbackMoveDBFailure(true, true))
    {
        return false;
    }
    if (!ShouldEmitDropMessages(100) || ShouldEmitDropMessages(0))
    {
        return false;
    }
    if (DropQuantityDelta(7) != -7)
    {
        return false;
    }
    if (!ShouldRejectRecycleNullSource(false) || ShouldRejectRecycleNullSource(true))
    {
        return false;
    }
    if (!ShouldRejectRecycleBusy(true) || ShouldRejectRecycleBusy(false))
    {
        return false;
    }
    if (!ShouldUseSimpleRecycleMove(1) || ShouldUseSimpleRecycleMove(0))
    {
        return false;
    }
    if (!ShouldRejectRecycleMoveFailed(255) || ShouldRejectRecycleMoveFailed(3))
    {
        return false;
    }
    if (!ShouldRollbackRecycleDBFailure(false, true) || ShouldRollbackRecycleDBFailure(true, true))
    {
        return false;
    }
    if (!IsRecycleBinSlideSourceSlot(2) || !IsRecycleBinSlideSourceSlot(10) || IsRecycleBinSlideSourceSlot(1) || IsRecycleBinSlideSourceSlot(11))
    {
        return false;
    }
    if (RecycleBinSlideTarget(5) != 4 || RecycleBinFullInsertSlot != 10)
    {
        return false;
    }
    if (!ShouldSkipEmptySlideSlot(false) || ShouldSkipEmptySlideSlot(true))
    {
        return false;
    }
    if (!IsEmptyRecycleBinSlot(1, 10) || !IsEmptyRecycleBinSlot(10, 10) || IsEmptyRecycleBinSlot(0, 10) || IsEmptyRecycleBinSlot(11, 10))
    {
        return false;
    }
    return true;
}
} // namespace

auto runInventoryMove1519SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "inventory move 1519 self-test failed\n";
    }
    return ok;
}
