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

// Keeps GUILD_BUY's host-independent processing decisions testable. Item
// lookup, NPC lookup, Lua execution, packet delivery, and logging stay in the
// packet handler.
namespace guildbuyhelpers
{
enum class PreScriptAction : uint8
{
    None,
    RejectStackLimit,
    CallScript,
};

struct ResultFields
{
    uint16 itemNo;
    uint8  count;
    uint8  trade;
};

constexpr auto SelectPreScriptAction(const bool itemResolved, const uint8 quantity, const uint32 stackSize) -> PreScriptAction
{
    if (!itemResolved)
    {
        return PreScriptAction::None;
    }

    return quantity > stackSize ? PreScriptAction::RejectStackLimit : PreScriptAction::CallScript;
}

constexpr auto ShouldSendScriptResult(const bool npcResolved, const bool scriptResultValid) -> bool
{
    return npcResolved && scriptResultValid;
}

constexpr auto MakeResultFields(const uint16 itemNo, const uint8 count, const int32 trade) -> ResultFields
{
    return { itemNo, count, static_cast<uint8>(trade) };
}
} // namespace guildbuyhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00AA
// This packet is sent by the client when buying items from a guild shop.
GP_CLI_PACKET(GP_CLI_COMMAND_GUILD_BUY,
              uint16_t ItemNo;            // The item id being purchased.
              uint8_t  PropertyItemIndex; // The index within the clients inventory to place the item.
              uint8_t  ItemNum;           // The quantity of items being purchased.
);
