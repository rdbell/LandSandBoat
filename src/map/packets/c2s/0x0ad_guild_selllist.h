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

#include "packets/s2c/0x085_guild_selllist.h"

#include <optional>
#include <vector>

// Keeps the process-time NPC guard and Lua result conversion independently
// testable. Packet validation remains owned by
// GP_CLI_COMMAND_GUILD_SELLLIST::validate.
namespace guildselllisthelpers
{
struct SourceEntry
{
    uint16_t ItemNo = 0;
    uint8_t  Count  = 0;
    uint8_t  Max    = 0;
    int32_t  Price  = 0;
};

struct ResponsePlan
{
    bool                      sendResponse = false;
    std::vector<GP_GUILD_ITEM> items;
};

inline auto BuildResponsePlan(const bool hasNpc, const std::vector<std::optional<SourceEntry>>& entries) -> ResponsePlan
{
    if (!hasNpc)
    {
        return {};
    }

    ResponsePlan plan;
    plan.sendResponse = true;
    plan.items.reserve(entries.size());
    for (const auto& source : entries)
    {
        if (!source.has_value())
        {
            continue;
        }

        plan.items.push_back({ source->ItemNo, source->Count, source->Max, source->Price });
    }
    return plan;
}
} // namespace guildselllisthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00AD
// This packet is sent by the client when requesting the current guild stock.
// (When opening the Sell window.)
GP_CLI_PACKET(GP_CLI_COMMAND_GUILD_SELLLIST);
