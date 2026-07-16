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
#include <algorithm>

#include "base.h"

namespace bazaarexithelpers
{
struct StateTransition
{
    bool removeBuyerCustomer;
    bool notifySeller;
    bool cleanBazaarID;
};

// SelectStateTransition keeps the process-time lookup and identity ordering
// explicit. A missing target returns before BazaarID is cleaned; a resolved
// target with a reused identity still cleans BazaarID but changes nothing on
// the target.
inline auto SelectStateTransition(const bool sellerResolved, const bool sellerIDMatches, const bool buyerGMHidden, const uint8_t buyerGMLevel, const uint8_t sellerGMLevel) -> StateTransition
{
    if (!sellerResolved)
    {
        return { false, false, false };
    }

    if (!sellerIDMatches)
    {
        return { false, false, true };
    }

    return { true, !buyerGMHidden || sellerGMLevel >= buyerGMLevel, true };
}

template <typename CustomerEntries>
inline void RemoveBuyerCustomers(CustomerEntries& customers, const uint32_t buyerID)
{
    std::erase_if(customers, [buyerID](const auto& customer) {
        return customer.id == buyerID;
    });
}
} // namespace bazaarexithelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0104
// This packet is sent by the client when exiting a bazaar.
GP_CLI_PACKET(GP_CLI_COMMAND_BAZAAR_EXIT);
