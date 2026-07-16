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

namespace guildsellhelpers
{

enum class Action : uint8
{
    None,
    RejectOverStack,
    SendScriptResult,
};

// RuntimeFacts are the host-owned lookup and script callback facts consumed by
// process. NPC lookup, Lua execution, packet delivery, and audit persistence
// remain map-host responsibilities.
struct RuntimeFacts
{
    bool   itemFound;
    uint32 itemStackSize;
    bool   guildShopNpcFound;
    bool   scriptResultValid;
    bool   auditPlayerVendor;
    uint16 scriptItemNo;
    uint8  scriptCount;
    int32  scriptTrade;
    uint8  scriptSold;
    uint32 scriptPrice;
};

struct Plan
{
    Action action;
    uint8  stock;
    uint16 itemNo;
    uint8  trade;
    bool   auditSale;
    uint16 auditItemNo;
    uint32 auditBasePrice;
    uint8  auditQuantity;
};

// MakePlan mirrors GP_CLI_COMMAND_GUILD_SELL::process's ordered item, stack,
// NPC, and Lua-result branches. scriptTrade deliberately converts to uint8,
// matching the S2C packet constructor's quantity parameter.
[[nodiscard]] constexpr auto MakePlan(uint8 itemNum, const RuntimeFacts& facts) -> Plan
{
    if (!facts.itemFound)
    {
        return { Action::None, 0, 0, 0, false, 0, 0, 0 };
    }
    if (itemNum > facts.itemStackSize)
    {
        return { Action::RejectOverStack, 0, 0, static_cast<uint8>(-4), false, 0, 0, 0 };
    }
    if (!facts.guildShopNpcFound || !facts.scriptResultValid)
    {
        return { Action::None, 0, 0, 0, false, 0, 0, 0 };
    }
    return { Action::SendScriptResult,
             facts.scriptCount,
             facts.scriptItemNo,
             static_cast<uint8>(facts.scriptTrade),
             facts.auditPlayerVendor && facts.scriptSold > 0,
             facts.scriptItemNo,
             facts.scriptPrice,
             facts.scriptSold };
}

} // namespace guildsellhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00AC
// This packet is sent by the client when selling items to a guild shop.
GP_CLI_PACKET(GP_CLI_COMMAND_GUILD_SELL,
              uint16_t ItemNo;            // The item id being sold.
              uint8_t  PropertyItemIndex; // The index within the clients inventory of the item being sold.
              uint8_t  ItemNum;           // The quantity of items being sold.
);
