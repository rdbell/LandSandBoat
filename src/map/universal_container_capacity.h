#pragma once

#include <cstdint>

// Pure CUContainer::SetItem / ClearSlot / IsSlotEmpty / IsContainerEmpty policy
// helpers.
// Host injects range / lock / occupancy scalars; helpers never touch CItem* or
// container storage.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2801: SetItem admission + PlanSetItemCountDelta
//   - 2813: ClearSlot range gate + no-count-adjust quirk
//   - 2822: PlanIsSlotEmpty
//   - 2829: IsContainerTypeEmpty
//   - 2965: ShouldAllowSetItem (SetItem outer gate)
//
// Production host: CUContainer::SetItem (universal_container.cpp) injects
// slotID < m_PItem.size() and m_lock into ShouldAllowSetItem, then
// PlanSetItemCountDelta on admit.
// Go dual-wire: universalcontainer.ShouldAllowSetItem
// (internal/universalcontainer/set_item.go). Prior pure port: slice 2801.

namespace ucontainerhelpers
{

// ShouldAllowSetItem mirrors the SetItem outer gate:
//   slotID < m_PItem.size() && !m_lock
//
// Formula (slice 2965 dual-wire):
//   slotInRange && !locked
//
// slotInRange — host-evaluated slotID < m_PItem.size()
// locked      — host-evaluated m_lock
// true  → host may apply PlanSetItemCountDelta and assign m_PItem[slotID]
// false → host leaves state unchanged and returns false
//
// Dual-wire of Go universalcontainer.ShouldAllowSetItem
// (internal/universalcontainer/set_item.go).
// Call site: CUContainer::SetItem before count delta / assignment.
// Host injects each conjunct after size/lock probes.
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
// Residual pure port: slice 2801 (paired with ShouldAllowSetItem).
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

// IsContainerTypeEmpty mirrors IsContainerEmpty type gate (slice 2829):
//   m_ContainerType == UCONTAINER_EMPTY
// Host injects the empty-type constant so the helper stays free of enum headers.
inline auto IsContainerTypeEmpty(const std::uint8_t type, const std::uint8_t emptyType) -> bool
{
    return type == emptyType;
}

} // namespace ucontainerhelpers
