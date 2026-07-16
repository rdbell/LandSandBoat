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

#include "base.h"

namespace shopsellsethelpers
{

enum class Action : uint8
{
    InvalidGil,
    InvalidItem,
    InvalidQuantity,
    InsufficientQuantity,
    ItemMismatch,
    LockedItem,
    ReservedItem,
    RemoveFailed,
    CompleteSale,
};

struct RuntimeFacts
{
    bool     gilItemValid;
    bool     itemResolved;
    uint32   quantity;
    uint32   itemStackSize;
    uint32   itemQuantity;
    uint16   requestedItemId;
    uint16   currentItemId;
    bool     itemLocked;
    uint32   itemReserve;
    bool     removeSucceeded;
    uint32   basePrice;
};

struct Plan
{
    Action action;
    uint32 saleCost;
};

// MakePlan mirrors the ordered host-independent guards in process. Inventory
// access, UpdateItem calls, audit persistence, and packets remain map-host
// responsibilities.
[[nodiscard]] constexpr auto MakePlan(const RuntimeFacts& facts) -> Plan
{
    if (!facts.gilItemValid)
    {
        return { Action::InvalidGil, 0 };
    }
    if (!facts.itemResolved)
    {
        return { Action::InvalidItem, 0 };
    }
    if (facts.quantity < 1 || facts.quantity > facts.itemStackSize)
    {
        return { Action::InvalidQuantity, 0 };
    }
    if (facts.quantity > facts.itemQuantity)
    {
        return { Action::InsufficientQuantity, 0 };
    }
    if (facts.requestedItemId != facts.currentItemId)
    {
        return { Action::ItemMismatch, 0 };
    }
    if (facts.itemLocked)
    {
        return { Action::LockedItem, 0 };
    }
    if (facts.itemReserve > 0)
    {
        return { Action::ReservedItem, 0 };
    }
    if (!facts.removeSucceeded)
    {
        return { Action::RemoveFailed, 0 };
    }
    return { Action::CompleteSale, facts.quantity * facts.basePrice };
}

} // namespace shopsellsethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0085
// This packet is sent by the client when confirming the sale of an item to a shop.
GP_CLI_PACKET(GP_CLI_COMMAND_SHOP_SELL_SET,
              uint16_t SellFlag;  // PS2: SellFlag
              uint16_t padding00; // PS2: (New; did not exist.)
);
