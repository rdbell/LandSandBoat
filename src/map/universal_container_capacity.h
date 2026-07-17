#pragma once

#include <cstdint>

// Pure CUContainer::SetItem admission and m_count delta policy (slice 2801).
//
// Production host: CUContainer::SetItem in universal_container.cpp.

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

} // namespace ucontainerhelpers
