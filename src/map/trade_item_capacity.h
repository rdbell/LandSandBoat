#pragma once

#include <cstdint>

// Pure CanTrade / DoTrade / AddItem eligibility policy from charutils.

namespace tradeitemhelpers
{

// Trade UContainer slots scanned: 0..8 inclusive.
constexpr std::uint8_t TradeSlotMin = 0;
constexpr std::uint8_t TradeSlotMax = 8;

// ERROR_SLOTID pin commonly 0xFF.
constexpr std::uint8_t ErrorSlotID = 0xFF;

// ShouldRejectMonstrosityTrade mirrors either side has monstro.
constexpr auto ShouldRejectMonstrosityTrade(const bool charIsMonstro, const bool targetIsMonstro) -> bool
{
    return charIsMonstro || targetIsMonstro;
}

// ShouldRejectTradeForSpace mirrors target free slots < trade item count.
constexpr auto ShouldRejectTradeForSpace(const std::uint8_t targetFreeSlots, const std::uint8_t tradeItemCount) -> bool
{
    return targetFreeSlots < tradeItemCount;
}

// ShouldRejectRareDuplicate mirrors rare flag && target already has item.
constexpr auto ShouldRejectRareDuplicate(const bool isRare, const bool targetHasItem) -> bool
{
    return isRare && targetHasItem;
}

// IsTradeSlotInRange mirrors slotid <= 8.
constexpr auto IsTradeSlotInRange(const std::uint8_t slotID) -> bool
{
    return slotID <= TradeSlotMax;
}

// ShouldCloneSingleStackTrade mirrors stackSize == 1 && reserve == 1.
constexpr auto ShouldCloneSingleStackTrade(const std::uint16_t stackSize, const std::uint32_t reserve) -> bool
{
    return stackSize == 1 && reserve == 1;
}

// TradeRemoveQuantity is 0 - amount for UpdateItem delta.
constexpr auto TradeRemoveQuantity(const std::uint32_t reserve) -> std::int32_t
{
    return 0 - static_cast<std::int32_t>(reserve);
}

// ShouldRejectAddItemEmptyOrZero mirrors freeSlots == 0 || quantity == 0.
constexpr auto ShouldRejectAddItemEmptyOrZero(const std::uint8_t freeSlots, const std::uint32_t quantity) -> bool
{
    return freeSlots == 0 || quantity == 0;
}

// ShouldRejectAddItemMissingDB mirrors spawn returned null.
constexpr auto ShouldRejectAddItemMissingDB(const bool itemSpawned) -> bool
{
    return !itemSpawned;
}

// ShouldUpdateCurrencyInstead mirrors isType(ITEM_CURRENCY).
constexpr auto ShouldUpdateCurrencyInstead(const bool isCurrency) -> bool
{
    return isCurrency;
}

// ShouldRejectRareAddItem mirrors rare flag && already has item.
constexpr auto ShouldRejectRareAddItem(const bool isRare, const bool alreadyHas) -> bool
{
    return isRare && alreadyHas;
}

// CurrencyInventorySlot is always 0 for gil updates.
constexpr std::uint8_t CurrencyInventorySlot = 0;

} // namespace tradeitemhelpers
