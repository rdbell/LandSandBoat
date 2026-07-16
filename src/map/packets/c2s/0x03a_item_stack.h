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

#include <vector>

#include "base.h"

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x003A
// This packet is sent by the client when sorting an inventory container.
namespace itemstackhelpers
{
struct StackSlot
{
    uint16_t itemID    = 0;
    uint32_t quantity  = 0;
    uint32_t stackSize = 0;
    bool     present   = false;
    bool     reserved  = false;
    bool     locked    = false;
};

struct StackTransfer
{
    uint8_t  destinationSlot = 0;
    uint8_t  sourceSlot      = 0;
    uint32_t quantity        = 0;
};

// BuildStackTransfers mirrors ITEM_STACK's ordered in-container merge loop.
// Slots are zero-based in the input; emitted packet/storage slots remain one-based.
[[nodiscard]] inline auto BuildStackTransfers(std::vector<StackSlot> slots) -> std::vector<StackTransfer>
{
    std::vector<StackTransfer> transfers;

    for (std::size_t slot = 0; slot < slots.size(); ++slot)
    {
        auto& destination = slots[slot];
        if (!destination.present || destination.reserved || destination.locked || destination.quantity >= destination.stackSize)
        {
            continue;
        }

        for (std::size_t sourceSlot = slot + 1; sourceSlot < slots.size(); ++sourceSlot)
        {
            auto& source = slots[sourceSlot];
            if (!source.present || source.itemID != destination.itemID || source.reserved || source.locked || source.quantity >= source.stackSize)
            {
                continue;
            }

            const uint32_t totalQuantity = destination.quantity + source.quantity;
            const uint32_t moveQuantity  = totalQuantity >= destination.stackSize ? destination.stackSize - destination.quantity : source.quantity;
            if (moveQuantity == 0)
            {
                continue;
            }

            transfers.push_back({ static_cast<uint8_t>(slot + 1), static_cast<uint8_t>(sourceSlot + 1), moveQuantity });
            destination.quantity += moveQuantity;
            source.quantity -= moveQuantity;
            if (source.quantity == 0)
            {
                source.present = false;
            }
        }
    }

    return transfers;
}
} // namespace itemstackhelpers

GP_CLI_PACKET(GP_CLI_COMMAND_ITEM_STACK,
              uint32_t Category; // PS2: Category
);
