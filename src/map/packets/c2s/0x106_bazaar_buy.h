/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#pragma once
#include <limits>

#include "base.h"

// Keeps BAZAAR_BUY's process-time guards and price calculation independently
// testable. Entity/storage lookup, item transfer, persistence, and packet
// delivery remain owned by GP_CLI_COMMAND_BAZAAR_BUY::process.
namespace bazaarbuyhelpers
{
enum class Action : uint8
{
    None,
    SendError,
    Purchase,
};

enum class Error : uint8
{
    None,
    SelfOrInventoryFull,
    NoUsableGil,
    UnavailableItem,
    PriceOverflow,
    InsufficientGil,
};

struct Facts
{
    bool   hasTarget;
    bool   targetMatchesBazaarID;
    bool   hasBazaarInventory;
    bool   hasBuyerInventory;
    bool   hasBazaarItem;
    bool   bazaarItemReserved;
    bool   buyerIsSeller;
    bool   buyerInventoryFull;
    bool   hasUsableGil;
    uint32 itemPrice;
    uint32 itemQuantity;
    uint32 buyerGil;
    uint16 tax;
};

struct Decision
{
    Action action;
    Error  error;
    uint32 sellerCredit;
    uint32 buyerDebit;
};

constexpr auto SelectDecision(const Facts& facts, const uint32 buyNum) -> Decision
{
    if (!facts.hasTarget || !facts.targetMatchesBazaarID || !facts.hasBazaarInventory || !facts.hasBuyerInventory ||
        !facts.hasBazaarItem || facts.bazaarItemReserved)
    {
        return { Action::None, Error::None, 0, 0 };
    }

    if (facts.buyerIsSeller || facts.buyerInventoryFull)
    {
        return { Action::SendError, Error::SelfOrInventoryFull, 0, 0 };
    }

    if (!facts.hasUsableGil)
    {
        return { Action::SendError, Error::NoUsableGil, 0, 0 };
    }

    if (facts.itemPrice == 0 || facts.itemQuantity < buyNum)
    {
        return { Action::SendError, Error::UnavailableItem, 0, 0 };
    }

    const uint64 basePrice  = static_cast<uint64>(facts.itemPrice) * buyNum;
    const uint64 totalPrice = (static_cast<uint64>(facts.tax) * basePrice) / 10000 + basePrice;
    if (totalPrice > std::numeric_limits<uint32>::max())
    {
        return { Action::SendError, Error::PriceOverflow, 0, 0 };
    }

    const auto priceWithTax = static_cast<uint32>(totalPrice);
    if (facts.buyerGil < priceWithTax)
    {
        return { Action::SendError, Error::InsufficientGil, 0, 0 };
    }

    return { Action::Purchase, Error::None, static_cast<uint32>(basePrice), priceWithTax };
}
} // namespace bazaarbuyhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0106
// This packet is sent by the client when requesting to purchase an item from a bazaar.
GP_CLI_PACKET(GP_CLI_COMMAND_BAZAAR_BUY,
              uint8_t  BazaarItemIndex; // PS2: BazaarItemIndex
              uint8_t  padding00[3];    // PS2: Dammy
              uint32_t BuyNum;          // PS2: BuyNum
);
