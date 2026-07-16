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

// Keeps BAZAAR_ITEMSET's process-time guards and effects independently
// testable. Inventory lookup, persistence, and packet delivery remain owned by
// GP_CLI_COMMAND_BAZAAR_ITEMSET::process.
namespace bazaaritemsethelpers
{
enum class Action : uint8
{
    None,
    ApplyPrice,
};

enum class SubType : uint8
{
    Unchanged,
    Locked,
    Unlocked,
};

struct Decision
{
    Action  action;
    uint32  price;
    SubType subType;
    bool    persistPrice;
    bool    sendItemAttr;
    bool    sendItemSame;
};

constexpr auto SelectDecision(
    const bool   hasStorage,
    const bool   hasItem,
    const bool   isReserved,
    const bool   isBusy,
    const bool   isExclusive,
    const bool   isLocked,
    const uint32 currentPrice,
    const uint32 requestedPrice) -> Decision
{
    if (!hasStorage || !hasItem || isReserved || isBusy || isExclusive || (isLocked && currentPrice == 0))
    {
        return { Action::None, 0, SubType::Unchanged, false, false, false };
    }

    return {
        Action::ApplyPrice,
        requestedPrice,
        requestedPrice == 0 ? SubType::Unlocked : SubType::Locked,
        true,
        true,
        true,
    };
}
} // namespace bazaaritemsethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x010A
// This packet is sent by the client when setting an items sale price within the players personal bazaar.
GP_CLI_PACKET(GP_CLI_COMMAND_BAZAAR_ITEMSET,
              uint8_t  ItemIndex;    // PS2: ItemIndex
              uint8_t  padding00[3]; // PS2: Dammy
              uint32_t Price;        // PS2: Price
);
