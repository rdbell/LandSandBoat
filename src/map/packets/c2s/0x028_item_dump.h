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

namespace itemdump
{

enum class Action : uint8_t
{
    Reject,
    Message,
    Drop,
    Recycle
};

struct Plan
{
    Action action{ Action::Reject };
    bool   breakLinkshell{};
};

auto PlanFor(uint8_t category, uint8_t index, uint32_t quantity, bool itemPresent, bool locked, uint32_t available, bool storedSlip, bool mainLinkshell, bool recycleEnabled, bool noRecycle) -> Plan;

} // namespace itemdump

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0028
// This packet is sent by the client when it drops an item.
GP_CLI_PACKET(GP_CLI_COMMAND_ITEM_DUMP,
              uint32_t ItemNum;   // PS2: ItemNum
              uint8_t  Category;  // PS2: Category
              uint8_t  ItemIndex; // PS2: ItemIndex
);
