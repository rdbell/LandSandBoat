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

} // namespace tradecontainerhelpers
