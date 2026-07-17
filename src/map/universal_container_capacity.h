#pragma once

#include <cstdint>

// Pure CUContainer::SetItem / ClearSlot policy helpers (slices 2801, 2813).
//
// Production host: CUContainer::{SetItem,ClearSlot} in universal_container.cpp.

namespace ucontainerhelpers
{

// ShouldAllowSetItem mirrors the SetItem outer gate:
//   slotID < m_PItem.size() && !m_lock
inline auto ShouldAllowSetItem(const bool slotInRange, const bool locked) -> bool
{
    return slotInRange && !locked;
}

// PlanSetItemCountDelta is the pure m_count adjustment for an admitted SetItem.
//
//   new non-null into empty slot → +1
//   null into occupied slot      → -1
//   otherwise                    → 0
//
// Replacing an occupied slot with a different non-null item is 0: LSB does not
// decrement then increment; both count branches are independent ifs.
inline auto PlanSetItemCountDelta(const bool newItemNonNull, const bool slotOccupied) -> std::int8_t
{
    if (newItemNonNull && !slotOccupied)
    {
        return 1;
    }
    if (!newItemNonNull && slotOccupied)
    {
        return -1;
    }
    return 0;
}

// ShouldClearSlot mirrors the ClearSlot range gate:
//   slotID < m_PItem.size()
//
// Unlike SetItem, ClearSlot does not consult m_lock.
inline auto ShouldClearSlot(const bool slotInRange) -> bool
{
    return slotInRange;
}

// ShouldAdjustCountOnClearSlot documents that ClearSlot does NOT change m_count
// (parity quirk vs SetItem(nullptr), which decrements when the slot was occupied).
inline auto ShouldAdjustCountOnClearSlot() -> bool
{
    return false;
}

// PlanIsSlotEmpty mirrors IsSlotEmpty: out of range is empty; in range is empty
// only when the slot pointer is null (slice 2822).
//   !slotInRange || itemNull
inline auto PlanIsSlotEmpty(const bool slotInRange, const bool itemNull) -> bool
{
    return !slotInRange || itemNull;
}

} // namespace ucontainerhelpers
