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

// CanCancelSale mirrors CancelSale's history index gate (slice 2920):
//   AucWorkIndex < history.size()
// Negative indexes (LSB packet handler should already reject -1) fail here too.
// Host injects AucWorkIndex and history length only (no CCharEntity* / vector).
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

} // namespace auctionutilshelpers
