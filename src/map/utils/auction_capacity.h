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

} // namespace auctionutilshelpers
