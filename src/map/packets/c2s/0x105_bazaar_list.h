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

// Keeps BAZAAR_LIST's process-time lookup, access, and packet predicates
// independently testable. Entity lookup, state mutation, and packet delivery
// remain owned by GP_CLI_COMMAND_BAZAAR_LIST::process.
namespace bazaarlisthelpers
{
enum class Lookup : uint8
{
    CurrentTarget,
    UniqueNo,
};

constexpr auto SelectLookup(const uint32 uniqueNo) -> Lookup
{
    return uniqueNo == 0 ? Lookup::CurrentTarget : Lookup::UniqueNo;
}

constexpr auto CanOpenBazaar(const bool sellerResolved, const uint32 sellerID, const uint32 uniqueNo, const bool sellerHasBazaar) -> bool
{
    return sellerResolved && sellerID == uniqueNo && sellerHasBazaar;
}

constexpr auto ShouldNotifySeller(const bool buyerGMHidden, const uint8 buyerGMLevel, const uint8 sellerGMLevel) -> bool
{
    return !buyerGMHidden || sellerGMLevel >= buyerGMLevel;
}

constexpr auto ShouldListItem(const bool itemPresent, const uint32 price) -> bool
{
    return itemPresent && price != 0;
}
} // namespace bazaarlisthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0105
// This packet is sent by the client when requesting to view a bazaar.
GP_CLI_PACKET(GP_CLI_COMMAND_BAZAAR_LIST,
              uint32_t UniqueNo;  // PS2: UniqueNo
              uint16_t ActIndex;  // PS2: ActIndex
              uint16_t padding00; // PS2: Dammy
);
