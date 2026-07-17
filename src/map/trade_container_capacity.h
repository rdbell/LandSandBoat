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
//   - 2962: ShouldAllowSetConfirmedStatus (setConfirmedStatus outer gate)
//
// Production host: CTradeContainer::setConfirmedStatus (trade_container.cpp)
// injects slotInRange / itemNonNull / quantityGteAmount into
// ShouldAllowSetConfirmedStatus, then ConfirmedStatusAmount on admit.
// Go dual-wire: tradecontainer.ShouldAllowSetConfirmedStatus
// (internal/tradecontainer/set_confirmed.go). Prior pure port: slice 2806.

namespace tradecontainerhelpers
{

// ShouldAllowSetConfirmedStatus mirrors the setConfirmedStatus outer gate:
//   slotID < m_PItem.size() && m_PItem[slotID] && quantity >= amount
//
// Formula (slice 2962 dual-wire):
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

// ShouldSetTradeItemEntry mirrors the multi-arg setItem outer gate:
//   slotId < m_PItem.size()
// Host injects slotInRange; helpers never touch CItem* or container storage.
inline auto ShouldSetTradeItemEntry(const bool slotInRange) -> bool
{
    return slotInRange;
}

// ShouldBumpItemsCountOnSetEntry is the pure m_ItemsCount += 1 gate once
// multi-arg setItem is admitted. Production always bumps when in range —
// including slot replace / clear — which is a known parity quirk.
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
