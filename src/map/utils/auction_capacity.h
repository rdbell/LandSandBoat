#pragma once

#include "common/cbasetypes.h"

// Pure auctionutils isPartiallyUsed policy helpers (slice 2854).
//
// Production host: anonymous isPartiallyUsed lambda in auctionutils.cpp
// (SellingItems / ProofOfPurchase call sites).
// Helpers take host-injected scalars only (no CItem* / CItemUsable* pointers).

namespace auctionutilshelpers
{

// IsPartiallyUsed mirrors the anonymous isPartiallyUsed lambda pure half:
//   isCharged && currentCharges < maxCharges
// Non-charged items are never partially used (charges ignored).
// Host injects ITEM_CHARGED subtype and getCurrentCharges/getMaxCharges.
inline auto IsPartiallyUsed(const bool isCharged, const uint8 currentCharges, const uint8 maxCharges) -> bool
{
    if (!isCharged)
    {
        return false;
    }

    return currentCharges < maxCharges;
}

// CanCancelSale mirrors CancelSale's history index gate
// (slice 3234 dedicated dual-wire; residual expand 2920 / pure 1135 —
// formula unchanged):
//   if aucWorkIndex < 0: return false
//   return aucWorkIndex < historyLen
// Production: AucWorkIndex < history.size() (negatives fail defensively;
// packet handler should already reject -1).
// Host injects AucWorkIndex and history length only (no CCharEntity* / vector).
// Dual-wire index: 3234 (dedicated expand residual 2920). Prior pure: 1135.
// Residual dual-wire suite: test_auction_cancel_sale_2920.
// Dedicated dual-wire suite: test_auctionutils_cancel_sale_3234.
inline auto CanCancelSale(const int aucWorkIndex, const int historyLen) -> bool
{
    if (aucWorkIndex < 0)
    {
        return false;
    }

    return aucWorkIndex < historyLen;
}

// CanAffordFee mirrors ProofOfPurchase's gil fee gate (slice 2924):
//   gilQuantity >= fee && gilReserve == 0
// Production rejects when quantity < fee || reserve > 0 (LotIn result 197).
// Host injects inventory slot-0 gil quantity/reserve and computed auctionFee
// only (no CItem* / CCharEntity* pointers).
inline auto CanAffordFee(const uint32 gilQuantity, const uint32 gilReserve, const uint32 fee) -> bool
{
    return gilQuantity >= fee && gilReserve == 0;
}

// HistoryCooldownMs is the OpenListOfSales refresh gate duration (5s).
// Dual-wire of Go auctionutils.HistoryCooldown / HistoryCooldownMs (slice 2935).
inline constexpr int64 HistoryCooldownMs = 5000;

// CanRefreshHistory mirrors OpenListOfSales cooldown pure half (slice 2935):
//   nowMs > lastMs + HistoryCooldownMs
// Production: curTick > m_AHHistoryTimestamp + 5s (strict greater-than).
// Equality at the boundary is still rate-limited (Info result 246).
// Host injects timer::now() and m_AHHistoryTimestamp as whole milliseconds
// only (no timer types / CCharEntity* on the pure surface).
inline auto CanRefreshHistory(const int64 nowMs, const int64 lastMs) -> bool
{
    return nowMs > lastMs + HistoryCooldownMs;
}

} // namespace auctionutilshelpers
