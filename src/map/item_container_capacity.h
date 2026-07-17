#pragma once

#include <cstdint>

// Pure CItemContainer size / insert / remove / move policy.
// Host injects scalars (size, count, occupancy, free slots); helpers never
// touch CItem* or container storage.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2802: size / insert / remove / move policy suite
//   - 2808 / 2823: SearchItemWithSpace / MatchesSearchItem gates
//   - 2826: FreeSlotsCount (unsigned size-minus-count)
//   - 2831: CanSearchSlotID (GetItem / search inclusive range; residual)
//   - 2942: CanInsertAtSlot residual dual-wire suite (InsertItem range gate)
//   - 2976: CanRemoveSlot residual dual-wire suite (RemoveItem range gate)
//   - 2989: ShouldDecrementCountOnRemove residual dual-wire suite (RemoveItem count drop)
//   - 3021: ShouldIncrementCountOnInsertAt residual dual-wire suite (InsertItem count bump)
//   - 3027: CanSetSize residual dual-wire suite (SetSize / AddSize acceptance)
//   - 3033: CanSearchSlotID (GetItem / search inclusive range dual-wire)
//   - 3038: MatchesSearchItem (SearchItem / SearchItems loop-body dual-wire)
//   - 3039: FreeSlotsCount (GetFreeSlotsCount size-minus-count dual-wire)
//   - 3164: CanSetSize dedicated dual-wire (can_set_size.go; expand residual 3027)
//   - 3194: CanInsertAtSlot earlier prior dedicated dual-wire (insert_slot.go; expand residual 2942)
//   - 3214: CanRemoveSlot dedicated dual-wire (remove_slot.go; expand residual 2976)
//   - 3270: CanInsertAtSlot prior dedicated dual-wire (insert_slot.go; expand residual 2942)
//   - 3301: CanInsertAtSlot dedicated dual-wire (insert_slot.go; expand residual 2942)
//   - 3351: ShouldDecrementCountOnRemove dedicated dual-wire (decrement_count_remove.go; expand residual 2989)
//   - 3374: ShouldIncrementCountOnInsertAt earlier prior dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//   - 3432: ShouldIncrementCountOnInsertAt earlier prior dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//   - 3486: ShouldIncrementCountOnInsertAt earlier prior dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//   - 3535: ShouldIncrementCountOnInsertAt earlier prior dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//   - 3579: ShouldIncrementCountOnInsertAt prior dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//   - 3624: ShouldIncrementCountOnInsertAt dedicated dual-wire (increment_count_insert.go; expand residual 3021)
//
// Dual-wire index:
//   - 2942: CanInsertAtSlot residual dual-wire suite
//   - 2976: CanRemoveSlot residual dual-wire suite
//   - 2989: ShouldDecrementCountOnRemove residual dual-wire suite
//   - 3021: ShouldIncrementCountOnInsertAt residual dual-wire suite
//   - 3027: CanSetSize residual dual-wire suite
//   - 3164: CanSetSize = newSize <= maxSize && newSize >= itemCount
//   - 3194: CanInsertAtSlot = slotID <= size (earlier prior dedicated expand residual 2942)
//   - 3214: CanRemoveSlot = slotID <= size
//   - 3270: CanInsertAtSlot = slotID <= size (prior dedicated expand residual 2942)
//   - 3301: CanInsertAtSlot = slotID <= size
//   - 3351: ShouldDecrementCountOnRemove = slotOccupied && slotID != 0
//   - 3374: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0 (earlier prior dedicated)
//   - 3432: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0 (earlier prior dedicated)
//   - 3486: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0 (earlier prior dedicated)
//   - 3535: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0 (earlier prior dedicated)
//   - 3579: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0 (prior dedicated)
//   - 3624: ShouldIncrementCountOnInsertAt = slotEmpty && slotID != 0
//
// Production host: CItemContainer::InsertItem(PItem, SlotID)
// (item_container.cpp) injects SlotID and m_size into CanInsertAtSlot.
// Go dual-wire: itemcontainer.CanInsertAtSlot
// (internal/itemcontainer/insert_slot.go).
// Residual dual-wire suite: 2942 (test_itemcontainer_insert_slot_2942).
// Prior dedicated dual-wire suites: 3194 / 3270
// (test_itemcontainer_can_insert_at_slot_3194 / _3270).
// Dedicated dual-wire suite: 3301 (test_itemcontainer_can_insert_at_slot_3301).
// Production host: CItemContainer::InsertItem injects emptiness and SlotID into
// ShouldIncrementCountOnInsertAt after CanInsertAtSlot admits.
// Go dual-wire: itemcontainer.ShouldIncrementCountOnInsertAt
// (internal/itemcontainer/increment_count_insert.go).
// Residual dual-wire suite: 3021 (test_item_increment_count_insert_3021).
// Prior dedicated dual-wire suites: 3374 / 3432 / 3486 / 3535 / 3579
// (test_item_increment_count_insert_3374 / _3432 / _3486 / _3535 / _3579).
// Dedicated dual-wire suite: 3624 (test_item_increment_count_insert_3624).
// Production host: CItemContainer::RemoveItem injects SlotID and m_size into
// CanRemoveSlot. Go dual-wire: itemcontainer.CanRemoveSlot
// (internal/itemcontainer/remove_slot.go).
// Residual dual-wire suite: 2976 (test_item_remove_slot_2976).
// Dedicated dual-wire suite: 3214 (test_itemcontainer_can_remove_slot_3214).
// Production host: CItemContainer::RemoveItem injects occupancy and SlotID into
// ShouldDecrementCountOnRemove after CanRemoveSlot admits.
// Go dual-wire: itemcontainer.ShouldDecrementCountOnRemove
// (internal/itemcontainer/decrement_count_remove.go).
// Residual dual-wire suite: 2989 (test_item_decrement_count_remove_2989).
// Dedicated dual-wire suite: 3351 (test_item_decrement_count_remove_3351).
// Production host: CItemContainer::SetSize / AddSize inject size / newsize,
// MAX_CONTAINER_SIZE, and m_count into CanSetSize.
// Go dual-wire: itemcontainer.CanSetSize
// (internal/itemcontainer/can_set_size.go).
// Residual dual-wire suite: 3027 (test_item_can_set_size_3027).
// Dedicated dual-wire suite: 3164 (test_itemcontainer_can_set_size_3164).
// Production host: CItemContainer::GetItem injects SlotID and m_size into
// CanSearchSlotID (shared inclusive bound with SearchItem / SearchItems /
// SearchItemWithSpace). Go dual-wire: itemcontainer.CanSearchSlotID
// (internal/itemcontainer/search_slot.go).
// Production host: CItemContainer::SearchItem / SearchItems inject occupancy
// and id match into MatchesSearchItem after null short-circuit.
// Go dual-wire: itemcontainer.MatchesSearchItem
// (internal/itemcontainer/matches_search_item.go).
// Production host: CItemContainer::GetFreeSlotsCount injects m_size and
// m_count into FreeSlotsCount (unsigned size-minus-count; wraps when
// count > size). Go dual-wire: itemcontainer.FreeSlotsCount
// (internal/itemcontainer/free_slots_count.go).

namespace itemcontainerhelpers
{

// ---------------------------------------------------------------------------
// Slice 3164 — SetSize / AddSize acceptance (dedicated expand residual 3027)
// ---------------------------------------------------------------------------

// CanSetSize mirrors SetSize / AddSize acceptance after the host computes the
// candidate size (AddSize must keep uint8 intermediate wrap on the host).
//
// Formula (slice 3164 dedicated dual-wire; residual expand 3027 / pure 2802 —
// formula unchanged):
//   newSize <= maxSize && newSize >= itemCount
//
// newSize   — host-evaluated candidate size (SetSize argument / AddSize wrap)
// maxSize   — host-evaluated MAX_CONTAINER_SIZE (typically 120)
// itemCount — host-evaluated m_count (tracked occupancy)
// true  → allow SetSize / AddSize (host assigns m_size)
// false → reject (return -1 / ERROR_SLOTID; above max or below item count)
//
// Dual-wire of Go itemcontainer.CanSetSize.
// Call sites: CItemContainer::SetSize and CItemContainer::AddSize.
// Prior pure port: slice 2802. Residual dual-wire suite: 3027 /
// test_item_can_set_size_3027. Dedicated dual-wire suite is
// test_itemcontainer_can_set_size_3164. Sibling dual-wire gates:
// CanInsertAtSlot (2942), CanRemoveSlot (2976),
// ShouldIncrementCountOnInsertAt (3021), ShouldDecrementCountOnRemove (2989).
inline auto CanSetSize(const std::uint8_t newSize, const std::uint8_t maxSize, const std::uint8_t itemCount) -> bool
{
    return newSize <= maxSize && newSize >= itemCount;
}

// ---------------------------------------------------------------------------
// Slice 3624 — InsertItem count bump (dedicated expand residual 3021;
// prior dedicated 3579 / 3535 / 3486 / 3432 / 3374)
// ---------------------------------------------------------------------------

// ShouldIncrementCountOnInsertAt mirrors InsertItem(PItem, SlotID) count bump:
// only empty nonzero slots contribute to m_count.
//
// Formula (slice 3624 dedicated dual-wire expand residual 3021; prior dedicated
// 3579 / 3535 / 3486 / 3432 / 3374 / pure 2802 — formula unchanged):
//   slotEmpty && slotID != 0
//
// slotEmpty — host-evaluated emptiness (m_ItemList[SlotID] == nullptr)
// slotID    — host-evaluated InsertItem SlotID
// true  → host increments m_count
// false → leave m_count unchanged (occupied slot or slot 0)
//
// Dual-wire of Go itemcontainer.ShouldIncrementCountOnInsertAt.
// Call site: CItemContainer::InsertItem(PItem, SlotID) after CanInsertAtSlot admits.
// Prior pure port: slice 2802. Residual dual-wire suite: 3021 /
// test_item_increment_count_insert_3021. Prior dedicated dual-wire suites: 3374 /
// test_item_increment_count_insert_3374, 3432 /
// test_item_increment_count_insert_3432, 3486 /
// test_item_increment_count_insert_3486, 3535 /
// test_item_increment_count_insert_3535, and 3579 /
// test_item_increment_count_insert_3579. Dedicated dual-wire suite is
// test_item_increment_count_insert_3624. Sibling dual-wire range gate:
// CanInsertAtSlot (slice 3301). Mirror decrement gate:
// ShouldDecrementCountOnRemove (slice 3351; slotOccupied && slotID != 0) —
// left residual under this slice. Sibling dual-wire gates: CanSetSize (3164),
// CanInsertAtSlot (3301), CanRemoveSlot (3214), ShouldDecrementCountOnRemove
// (3351) — left residual under this slice.
inline auto ShouldIncrementCountOnInsertAt(const bool slotEmpty, const std::uint8_t slotID) -> bool
{
    return slotEmpty && slotID != 0;
}

// ---------------------------------------------------------------------------
// Slice 3351 — RemoveItem count drop (dedicated expand residual 2989)
// ---------------------------------------------------------------------------

// ShouldDecrementCountOnRemove mirrors RemoveItem count drop: only occupied
// nonzero slots contribute to m_count.
//
// Formula (slice 3351 dedicated dual-wire; residual expand 2989 / pure 2802 —
// formula unchanged):
//   slotOccupied && slotID != 0
//
// slotOccupied — host-evaluated occupancy (m_ItemList[SlotID] != nullptr)
// slotID       — host-evaluated RemoveItem SlotID
// true  → host decrements m_count
// false → leave m_count unchanged (empty slot or slot 0)
//
// Dual-wire of Go itemcontainer.ShouldDecrementCountOnRemove.
// Call site: CItemContainer::RemoveItem after CanRemoveSlot admits.
// Prior pure port: slice 2802. Residual dual-wire suite: 2989 /
// test_item_decrement_count_remove_2989. Dedicated dual-wire suite is
// test_item_decrement_count_remove_3351. Sibling dual-wire range gate:
// CanRemoveSlot (slice 3214). Mirror increment gate:
// ShouldIncrementCountOnInsertAt (slice 3021; slotEmpty && slotID != 0) —
// left residual under this slice. Sibling dual-wire gates: CanSetSize (3164),
// CanInsertAtSlot (3301), CanRemoveSlot (3214) — left residual under this slice.
inline auto ShouldDecrementCountOnRemove(const bool slotOccupied, const std::uint8_t slotID) -> bool
{
    return slotOccupied && slotID != 0;
}

// ---------------------------------------------------------------------------
// Slice 3301 — InsertItem(PItem, SlotID) range gate (dedicated expand residual 2942)
// ---------------------------------------------------------------------------

// CanInsertAtSlot mirrors InsertItem(PItem, SlotID) range gate: reject when
// SlotID > size, so slotID <= size is accepted (including slot 0).
//
// Formula (slice 3301 dedicated dual-wire; residual expand 2942 / prior 3270 /
// prior 3194 / pure 2802 — formula unchanged):
//   slotID <= size
//
// slotID — host-evaluated explicit InsertItem SlotID
// size   — host-evaluated m_size (usable slot count)
// true  → allow InsertItem at SlotID (host still moves unique_ptr / sets location)
// false → reject with ERROR_SLOTID (out of range)
//
// Dual-wire of Go itemcontainer.CanInsertAtSlot.
// Call site: CItemContainer::InsertItem(PItem, SlotID) before ownership move.
// Prior pure port: slice 2802. Residual dual-wire suite: 2942 /
// test_itemcontainer_insert_slot_2942. Prior dedicated dual-wire suites: 3194 /
// test_itemcontainer_can_insert_at_slot_3194 and 3270 /
// test_itemcontainer_can_insert_at_slot_3270. Dedicated dual-wire suite is
// test_itemcontainer_can_insert_at_slot_3301. Sibling dual-wire gates:
// CanSetSize (3164), CanRemoveSlot (3214),
// ShouldIncrementCountOnInsertAt (3021) — left residual under this slice.
inline auto CanInsertAtSlot(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

// ---------------------------------------------------------------------------
// Slice 3214 — RemoveItem range gate (dedicated expand residual 2976)
// ---------------------------------------------------------------------------

// CanRemoveSlot mirrors RemoveItem range gate: reject when SlotID > size,
// so slotID <= size is accepted (including slot 0).
//
// Formula (slice 3214 dedicated dual-wire; residual expand 2976 / pure 2802 —
// formula unchanged):
//   slotID <= size
//
// slotID — host-evaluated RemoveItem SlotID
// size   — host-evaluated m_size (usable slot count)
// true  → allow RemoveItem at SlotID (host still moves unique_ptr / drops count)
// false → reject (return nullptr; out of range)
//
// Dual-wire of Go itemcontainer.CanRemoveSlot.
// Call site: CItemContainer::RemoveItem before ownership move.
// Prior pure port: slice 2802. Residual dual-wire suite: 2976 /
// test_item_remove_slot_2976. Dedicated dual-wire suite is
// test_itemcontainer_can_remove_slot_3214. Sibling dual-wire gates:
// CanSetSize (3164), CanInsertAtSlot (3301),
// ShouldDecrementCountOnRemove (2989) — left residual under this slice.
// Same predicate as CanInsertAtSlot (slice 3301) and CanSearchSlotID (slice 3033).
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

// CanSearchSlotID is the pure addressable-slot range for GetItem and the
// inclusive scan bound shared with SearchItem / SearchItems /
// SearchItemWithSpace: accept when slotID <= size (including slot 0).
//
// Formula (slice 3033 dual-wire):
//   slotID <= size
//
// slotID — host-evaluated GetItem SlotID (or search scan candidate)
// size   — host-evaluated m_size (usable slot count)
// true  → allow GetItem at SlotID (host still returns m_ItemList entry / null)
// false → reject (return nullptr; out of range)
//
// Dual-wire of Go itemcontainer.CanSearchSlotID.
// Call site: CItemContainer::GetItem before m_ItemList lookup.
// Prior pure port: slice 2831. Sibling dual-wire range gates: CanInsertAtSlot
// (slice 2942), CanRemoveSlot (slice 2976). Same predicate; named for the
// lookup/search host path.
inline auto CanSearchSlotID(const std::uint8_t slotID, const std::uint8_t size) -> bool
{
    return slotID <= size;
}

// MatchesSearchItem is the pure loop-body gate for SearchItem / SearchItems:
// occupied slot and matching item id.
//
// Formula (slice 3038 dual-wire):
//   slotOccupied && idMatches
//
// slotOccupied — host-evaluated occupancy (m_ItemList[slotId] != nullptr)
// idMatches    — host-evaluated item id equality (getID() == itemId)
// true  → host treats the slot as a SearchItem / SearchItems hit
// false → continue scan (empty slot or id mismatch)
//
// Dual-wire of Go itemcontainer.MatchesSearchItem.
// Call sites: CItemContainer::SearchItem / SearchItems after null short-circuit.
// Prior pure port: slice 2823. Sibling dual-wire range gate: CanSearchSlotID
// (slice 3033). Related residual WithSpace gate: MatchesSearchItemWithSpace
// (slice 2808; adds HasSpaceForQuantity).
inline auto MatchesSearchItem(const bool slotOccupied, const bool idMatches) -> bool
{
    return slotOccupied && idMatches;
}

// HasSpaceForQuantity mirrors SearchItemWithSpace's unsigned stack room check:
// quantity <= stackSize - requestQuantity. Host must keep uint32 types so
// requestQuantity > stackSize wraps the subtraction (underflow) and can match.
inline auto HasSpaceForQuantity(
    const std::uint32_t quantity,
    const std::uint32_t stackSize,
    const std::uint32_t requestQuantity) -> bool
{
    return quantity <= stackSize - requestQuantity;
}

// MatchesSearchItemWithSpace is the pure loop-body gate for SearchItemWithSpace:
// occupied slot, matching item id, and HasSpaceForQuantity.
inline auto MatchesSearchItemWithSpace(
    const bool          slotOccupied,
    const bool          idMatches,
    const std::uint32_t quantity,
    const std::uint32_t stackSize,
    const std::uint32_t requestQuantity) -> bool
{
    return slotOccupied && idMatches && HasSpaceForQuantity(quantity, stackSize, requestQuantity);
}

// FreeSlotsCount mirrors GetFreeSlotsCount: unsigned size-minus-count.
//
// Formula (slice 3039 dual-wire):
//   size - count
//
// size  — host-evaluated m_size (usable slot count)
// count — host-evaluated m_count (tracked occupancy)
// result — free slots as uint8; when count > size, subtraction wraps
//          (uint8 underflow, e.g. size 0 count 1 → 255)
//
// Dual-wire of Go itemcontainer.FreeSlotsCount.
// Call site: CItemContainer::GetFreeSlotsCount — host injects m_size / m_count.
// Prior pure port: slice 2826. Callers that pass free slots into other pure
// helpers (e.g. PlanMoveItemTo) inject the host result; they do not re-implement
// the formula.
inline auto FreeSlotsCount(const std::uint8_t size, const std::uint8_t count) -> std::uint8_t
{
    return size - count;
}

} // namespace itemcontainerhelpers
