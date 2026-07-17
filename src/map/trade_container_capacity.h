#pragma once

#include <cstdint>

// Pure CTradeContainer admission and count-bump policy helpers.
// Host injects range / non-null / quantity-gate scalars; helpers never touch
// CItem* or container storage.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2806: setConfirmedStatus admission + ConfirmedStatusAmount
//   - 2812: multi-arg setItem entry + ItemsCount bump
//   - 2821: TradeSlotTotalContribution (getTotalQuantity per-slot term)
//   - 2824: TradeSlotCountsTowardSlotCount (getSlotCount occupancy)
//   - 2830: TradeSlotMatchesItemID (getItemQuantity match gate)
//   - 2962: ShouldAllowSetConfirmedStatus residual dual-wire suite
//   - 2984: ShouldSetTradeItemEntry residual dual-wire suite
//   - 2997: ShouldBumpItemsCountOnSetEntry (multi-arg setItem ItemsCount bump)
//   - 3175: ShouldAllowSetConfirmedStatus dedicated dual-wire
//           (set_confirmed.go; expand residual 2962)
//   - 3211: ShouldSetTradeItemEntry dedicated dual-wire
//           (set_item_entry.go; expand residual 2984)
//
// Dual-wire index:
//   - 2962: ShouldAllowSetConfirmedStatus residual dual-wire suite
//   - 2984: ShouldSetTradeItemEntry residual dual-wire suite
//   - 3175: ShouldAllowSetConfirmedStatus =
//           slotInRange && itemNonNull && quantityGteAmount
//   - 3211: ShouldSetTradeItemEntry = slotInRange
//
// Production host: CTradeContainer::setConfirmedStatus (trade_container.cpp)
// injects slotInRange / itemNonNull / quantityGteAmount into
// ShouldAllowSetConfirmedStatus, then ConfirmedStatusAmount on admit.
// Go dual-wire: tradecontainer.ShouldAllowSetConfirmedStatus
// (internal/tradecontainer/set_confirmed.go).
// Residual dual-wire suite: 2962 (test_trade_set_confirmed_2962).
// Dedicated dual-wire suite: 3175 (test_tradecontainer_set_confirmed_3175).
// Prior pure port: slice 2806.
//
// Production host: CTradeContainer::setItem multi-arg (trade_container.cpp)
// injects slotId < m_PItem.size() into ShouldSetTradeItemEntry, then
// ShouldBumpItemsCountOnSetEntry + assign on admit.
// Go dual-wire: tradecontainer.ShouldSetTradeItemEntry
// (internal/tradecontainer/set_item_entry.go).
// Residual dual-wire suite: 2984 (test_trade_set_item_entry_2984).
// Dedicated dual-wire suite: 3211 (test_tradecontainer_set_trade_item_entry_3211).
// Prior pure port: slice 2812.
// Go dual-wire: tradecontainer.ShouldBumpItemsCountOnSetEntry
// (internal/tradecontainer/bump_items_count.go). Prior pure port: slice 2812.

namespace tradecontainerhelpers
{

// ---------------------------------------------------------------------------
// Slice 3175 — setConfirmedStatus outer gate (dedicated expand residual 2962)
// ---------------------------------------------------------------------------

// ShouldAllowSetConfirmedStatus mirrors the setConfirmedStatus outer gate:
//   slotID < m_PItem.size() && m_PItem[slotID] && quantity >= amount
//
// Formula (slice 3175 dedicated dual-wire; residual expand 2962 / pure 2806 —
// formula unchanged):
//   slotInRange && itemNonNull && quantityGteAmount
//
// slotInRange       — host-evaluated slotID < m_PItem.size()
// itemNonNull       — host-evaluated m_PItem[slotID] != nullptr (only if in range)
// quantityGteAmount — host-evaluated getQuantity() >= amount
//                     (inject false when item is null; never call getQuantity on null)
// true  → host may write m_confirmed[slotID] via ConfirmedStatusAmount and return true
// false → host leaves state unchanged and returns false
//
// Dual-wire of Go tradecontainer.ShouldAllowSetConfirmedStatus
// (internal/tradecontainer/set_confirmed.go).
// Call site: CTradeContainer::setConfirmedStatus before confirmed write.
// Prior pure port: slice 2806. Residual dual-wire suite: 2962 /
// test_trade_set_confirmed_2962. Dedicated dual-wire suite is
// test_tradecontainer_set_confirmed_3175. Sibling dual-wire gates:
// ShouldSetTradeItemEntry (3211 / residual 2984), ShouldBumpItemsCountOnSetEntry (2997).
// Host injects each conjunct after short-circuit-safe probes.
inline auto ShouldAllowSetConfirmedStatus(
    const bool slotInRange,
    const bool itemNonNull,
    const bool quantityGteAmount) -> bool
{
    return slotInRange && itemNonNull && quantityGteAmount;
}

// ConfirmedStatusAmount is the pure m_confirmed write value once admitted:
//   std::min(amount, itemQuantity)
// When the outer gate has already required quantity >= amount, this equals
// amount; min is preserved for parity with production assignment.
// Residual pure port: slice 2806 (paired with ShouldAllowSetConfirmedStatus).
inline auto ConfirmedStatusAmount(const std::uint32_t amount, const std::uint32_t itemQuantity) -> std::uint32_t
{
    return amount < itemQuantity ? amount : itemQuantity;
}

// ---------------------------------------------------------------------------
// Slice 3211 — multi-arg setItem outer gate (dedicated expand residual 2984)
// ---------------------------------------------------------------------------

// ShouldSetTradeItemEntry mirrors the multi-arg setItem outer gate:
//   slotId < m_PItem.size()
//
// Formula (slice 3211 dedicated dual-wire; residual expand 2984 / pure 2812 —
// formula unchanged):
//   ShouldSetTradeItemEntry(slotInRange) = slotInRange
//
// slotInRange — host-evaluated slotId < m_PItem.size()
// true  → host may bump m_ItemsCount (ShouldBumpItemsCountOnSetEntry) and assign
//         m_PItem / m_itemID / m_slotID / m_quantity at the slot
// false → host leaves state unchanged
//
// Dual-wire of Go tradecontainer.ShouldSetTradeItemEntry
// (internal/tradecontainer/set_item_entry.go).
// Call site: CTradeContainer::setItem multi-arg before count bump + assign.
// Prior pure port: slice 2812. Residual dual-wire suite: 2984 /
// test_trade_set_item_entry_2984. Dedicated dual-wire suite is
// test_tradecontainer_set_trade_item_entry_3211. Sibling dual-wire gates:
// ShouldAllowSetConfirmedStatus (3175), ShouldBumpItemsCountOnSetEntry (2997).
// Host injects slotInRange only; helpers never touch CItem* or container storage.
inline auto ShouldSetTradeItemEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// ShouldBumpItemsCountOnSetEntry is the pure m_ItemsCount += 1 gate once
// multi-arg setItem is admitted:
//   slotId < m_PItem.size()  (same inject as outer admission)
//
// Formula (slice 2997 dual-wire):
//   ShouldBumpItemsCountOnSetEntry(slotInRange) = slotInRange
//
// Production always bumps when in range — including slot replace / clear —
// which is a known parity quirk.
//
// slotInRange — host-evaluated slotId < m_PItem.size()
// true  → host may m_ItemsCount += 1 before assigning slot fields
// false → host does not bump (and outer gate already rejected the write)
//
// Dual-wire of Go tradecontainer.ShouldBumpItemsCountOnSetEntry
// (internal/tradecontainer/bump_items_count.go). Prior pure port: slice 2812.
// Call site: CTradeContainer::setItem multi-arg after ShouldSetTradeItemEntry
// admits. Sibling dual-wire: ShouldSetTradeItemEntry (slice 3211; residual 2984).
// Host injects slotInRange only; helpers never touch CItem* or container storage.
inline auto ShouldBumpItemsCountOnSetEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// TradeCurrencyItemID is the sentinel m_itemID value for gil / currency.
// Production getTotalQuantity treats this id as contributing 1 unit.
inline constexpr std::uint16_t TradeCurrencyItemID = 0xFFFF;

// TradeSlotTotalContribution is the pure per-slot term in getTotalQuantity:
//   m_itemID[slot] == 0xFFFF ? 1 : m_quantity[slot]
// Currency slots always contribute one item unit regardless of stored quantity.
// Host sums over slots; helper never touches container storage.
inline auto TradeSlotTotalContribution(const std::uint16_t itemID, const std::uint32_t quantity) -> std::uint32_t
{
    return itemID == TradeCurrencyItemID ? 1u : quantity;
}

// TradeSlotCountsTowardSlotCount mirrors getSlotCount occupancy:
//   m_itemID[slot] != 0  (slice 2824). Gil 0xFFFF still counts as occupied.
inline auto TradeSlotCountsTowardSlotCount(const std::uint16_t itemID) -> bool
{
    return itemID != 0;
}

// TradeSlotMatchesItemID is the pure per-slot match gate in getItemQuantity:
//   m_itemID[slot] == itemID  (slice 2830).
// When true the host contribution is m_quantity[slot]; when false, zero.
// Host sums over slots; helper never touches container storage.
inline auto TradeSlotMatchesItemID(const std::uint16_t slotItemID, const std::uint16_t targetID) -> bool
{
    return slotItemID == targetID;
}

} // namespace tradecontainerhelpers
