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

namespace tradelisthelpers
{

struct ItemFacts
{
    bool     exists{};
    bool     matchesRequestedID{};
    bool     exclusive{};
    bool     locked{};
    bool     linkshell{};
    bool     linkshellEquipped{};
    uint32_t requestedQuantity{};
    uint32_t reservedQuantity{};
    uint32_t availableQuantity{};
};

enum class Outcome
{
    InvalidTarget,
    InvalidItem,
    LinkshellNotEquipped,
    Update,
};

struct Plan
{
    Outcome outcome{ Outcome::InvalidTarget };
    bool    releaseExistingOffer{};
};

// makePlan captures the pure branch ordering of TRADE_LIST::process.
// Inventory mutation, packets, auditing, and container unlocking remain host-owned.
constexpr auto makePlan(bool targetsMatch, bool existingOffer, const ItemFacts& item) -> Plan
{
    if (!targetsMatch)
    {
        return {};
    }

    const auto releaseExistingOffer = existingOffer;
    if (!item.exists || !item.matchesRequestedID || item.exclusive ||
        item.requestedQuantity + item.reservedQuantity > item.availableQuantity || item.locked)
    {
        return { Outcome::InvalidItem, releaseExistingOffer };
    }

    if (item.linkshell && !item.linkshellEquipped)
    {
        return { Outcome::LinkshellNotEquipped, releaseExistingOffer };
    }

    return { Outcome::Update, releaseExistingOffer };
}

} // namespace tradelisthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0034
// This packet is sent by the client when setting an item inside of the trade window.
GP_CLI_PACKET(GP_CLI_COMMAND_TRADE_LIST,
              uint32_t ItemNum;    // PS2: ItemNum
              uint16_t ItemNo;     // PS2: ItemNo
              uint8_t  ItemIndex;  // PS2: ItemIndex
              uint8_t  TradeIndex; // PS2: TradeIndex
);
