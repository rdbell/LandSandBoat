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
//   - 2965: ShouldAllowSetItem residual dual-wire suite (SetItem outer gate)
//   - 2980: ShouldClearSlot residual dual-wire suite (ClearSlot range gate)
//   - 3176: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965)
//   - 3248: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965 / prior 3176)
//   - 3288: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965 / prior 3248)
//   - 3319: ShouldAllowSetItem dedicated dual-wire (set_item.go; residual 2965 / prior 3288)
//   - 3354: ShouldClearSlot dedicated dual-wire (clear_slot.go; residual 2813 / prior 2980)
//
// Dual-wire index:
//   - 2965: ShouldAllowSetItem residual dual-wire suite
//   - 3176: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965)
//   - 3248: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965 / prior 3176)
//   - 3288: ShouldAllowSetItem prior dedicated dual-wire (expand residual 2965 / prior 3248)
//   - 3319: ShouldAllowSetItem = slotInRange && !locked
//   - 2980: ShouldClearSlot residual dual-wire suite
//   - 3354: ShouldClearSlot = slotInRange
//
// Production host: CUContainer::SetItem (universal_container.cpp) injects
// slotID < m_PItem.size() and m_lock into ShouldAllowSetItem, then
// PlanSetItemCountDelta on admit.
// Go dual-wire: universalcontainer.ShouldAllowSetItem
// (internal/universalcontainer/set_item.go). Prior pure port: slice 2801.
// Residual dual-wire suite: 2965 (test_universal_set_item_2965).
// Prior dedicated dual-wire suites: 3176 (test_universalcontainer_allow_set_item_3176),
// 3248 (test_universalcontainer_set_item_3248),
// 3288 (test_universalcontainer_set_item_3288).
// Dedicated dual-wire suite: 3319 (test_universalcontainer_set_item_3319).
//
// Production host: CUContainer::ClearSlot (universal_container.cpp) injects
// slotID < m_PItem.size() into ShouldClearSlot (does not inject m_lock).
// Go dual-wire: universalcontainer.ShouldClearSlot
// (internal/universalcontainer/clear_slot.go). Prior pure port: slice 2813.
// Residual dual-wire suite: 2980 (test_universal_clear_slot_2980).
// Dedicated dual-wire suite: 3354 (test_universal_clear_slot_3354).

namespace ucontainerhelpers
{

// ---------------------------------------------------------------------------
// Slice 3319 — SetItem outer gate (dedicated expand residual 2965 / prior 3288)
// ---------------------------------------------------------------------------

// ShouldAllowSetItem mirrors the SetItem outer gate:
//   slotID < m_PItem.size() && !m_lock
//
// Formula (slice 3319 dedicated dual-wire; residual expand 2965 / prior 3288 /
// prior 3248 / prior 3176 / pure 2801 — formula unchanged):
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
// Prior pure port: slice 2801. Residual dual-wire suite: 2965 /
// test_universal_set_item_2965. Prior dedicated dual-wire suites: 3176 /
// test_universalcontainer_allow_set_item_3176, 3248 /
// test_universalcontainer_set_item_3248, 3288 /
// test_universalcontainer_set_item_3288. Dedicated dual-wire suite is
// test_universalcontainer_set_item_3319. Sibling dual-wire:
// ShouldClearSlot (3354 / prior 2980).
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

// ---------------------------------------------------------------------------
// Slice 3354 — ClearSlot range gate (dedicated expand residual 2813 / prior 2980)
// ---------------------------------------------------------------------------

// ShouldClearSlot mirrors the ClearSlot range gate:
//   slotID < m_PItem.size()
//
// Formula (slice 3354 dedicated dual-wire; residual expand 2813 / prior 2980 —
// formula unchanged):
//   slotInRange
//
// slotInRange — host-evaluated slotID < m_PItem.size()
// true  → host may assign m_PItem[slotID] = nullptr
// false → host leaves state unchanged
//
// Unlike SetItem, ClearSlot does not consult m_lock.
// ClearSlot also does not adjust m_count (ShouldAdjustCountOnClearSlot is
// false; residual 2813).
//
// Dual-wire of Go universalcontainer.ShouldClearSlot
// (internal/universalcontainer/clear_slot.go).
// Call site: CUContainer::ClearSlot before null assignment.
// Host injects the range flag after the size probe.
// Prior pure port: slice 2813. Residual dual-wire suite: 2980 /
// test_universal_clear_slot_2980. Dedicated dual-wire suite is
// test_universal_clear_slot_3354. Sibling dual-wire:
// ShouldAllowSetItem (3319; leave residual — do not thrash set_item).
inline auto ShouldClearSlot(const bool slotInRange) -> bool
{
    return slotInRange;
}

// ShouldAdjustCountOnClearSlot documents that ClearSlot does NOT change m_count
// (parity quirk vs SetItem(nullptr), which decrements when the slot was occupied).
// Residual pure port: slice 2813 (paired with ShouldClearSlot dual-wire 3354 /
// prior 2980).
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
