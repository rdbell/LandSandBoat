#pragma once

#include <cstdint>

// Pure CTradeContainer::setConfirmedStatus admission and stored-amount policy.
// Host injects range / non-null / quantity-gate scalars; helpers never touch
// CItem* or container storage.

namespace tradecontainerhelpers
{

// ShouldAllowSetConfirmedStatus mirrors the setConfirmedStatus outer gate:
//   slotID < m_PItem.size() && m_PItem[slotID] && quantity >= amount
// Host injects each conjunct after short-circuit-safe probes (do not call
// getQuantity on a null item; inject quantityGteAmount=false when null).
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

} // namespace tradecontainerhelpers
