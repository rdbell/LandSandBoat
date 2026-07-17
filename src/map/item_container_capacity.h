#pragma once

#include <cstdint>

// Pure CItemContainer size / insert / remove / move policy.
// Host injects scalars (size, count, occupancy, free slots); helpers never
// touch CItem* or container storage.

namespace itemcontainerhelpers
{

// CanSetSize mirrors SetSize / AddSize acceptance after the host computes the
// candidate size (AddSize must keep uint8 intermediate wrap on the host).
// Accepts when newSize <= maxSize && newSize >= itemCount.
inline auto CanSetSize(const std::uint8_t newSize, const std::uint8_t maxSize, const std::uint8_t itemCount) -> bool
{
    return newSize <= maxSize && newSize >= itemCount;
}

// ShouldIncrementCountOnInsertAt mirrors InsertItem(PItem, SlotID) count bump:
// only empty nonzero slots contribute to m_count.
inline auto ShouldIncrementCountOnInsertAt(const bool slotEmpty, const std::uint8_t slotID) -> bool
{
    return slotEmpty && slotID != 0;
}

// ShouldDecrementCountOnRemove mirrors RemoveItem count drop: only occupied
// nonzero slots contribute to m_count.
inline auto ShouldDecrementCountOnRemove(const bool slotOccupied, const std::uint8_t slotID) -> bool
{
    return slotOccupied && slotID != 0;
}

// CanInsertAtSlot mirrors InsertItem(PItem, SlotID) range gate: reject when
// SlotID > size, so slotID <= size is accepted (including slot 0).
inline auto CanInsertAtSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

// CanRemoveSlot mirrors RemoveItem range gate: same SlotID > size reject.
inline auto CanRemoveSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

// MoveItemToDisposition is the pure pre-remove disposition of MoveItemTo.
// Host still owns RemoveItem / InsertItem and post-remove null checks.
enum class MoveItemToDisposition : std::uint8_t
{
    Allow = 0,
    RejectOutOfRangeOrOccupied, // explicit dst: dstSlot > dstSize || occupied
    RejectNoFreeSlots,          // auto dst: free slots == 0
};

// PlanMoveItemTo short-circuits in production MoveItemTo order:
// 1) explicit dst → reject if out of range or occupied
// 2) auto dst → reject if free slots == 0
// 3) else allow (host then Remove + Insert)
inline auto PlanMoveItemTo(
    const bool        hasExplicitDst,
    const std::uint8_t dstSlot,
    const std::uint8_t dstSize,
    const bool        dstSlotOccupied,
    const std::uint8_t dstFreeSlots) -> MoveItemToDisposition
{
    if (hasExplicitDst)
    {
        if (dstSlot > dstSize || dstSlotOccupied)
        {
            return MoveItemToDisposition::RejectOutOfRangeOrOccupied;
        }
        return MoveItemToDisposition::Allow;
    }
    if (dstFreeSlots == 0)
    {
        return MoveItemToDisposition::RejectNoFreeSlots;
    }
    return MoveItemToDisposition::Allow;
}

} // namespace itemcontainerhelpers
