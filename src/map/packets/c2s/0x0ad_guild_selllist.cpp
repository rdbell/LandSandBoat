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

#include "0x0ad_guild_selllist.h"

#include "entities/char_entity.h"
#include "lua/luautils.h"
#include "utils/zoneutils.h"

// Go host pure half: packetsystem.ValidateGuildSellList / ProcessGuildSellList /
// NewGuildSellListHandler (6495); plan mappacket.ClientGuildSellListRuntimePlanFor.
auto GP_CLI_COMMAND_GUILD_SELLLIST::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustNotEqual(PChar->guildShopNpc_.id, 0, "Character does not have a guild shop");
}

void GP_CLI_COMMAND_GUILD_SELLLIST::process(MapSession* PSession, CCharEntity* PChar) const
{
    if (auto* PNpc = zoneutils::GetEntity(PChar->guildShopNpc_.id, TYPE_NPC))
    {
        const auto items = luautils::callGlobal<sol::table>("xi.guildShops.onSellList", PChar, PNpc);

        std::vector<std::optional<guildselllisthelpers::SourceEntry>> entries;
        entries.reserve(items.size());
        for (std::size_t i = 1; i <= items.size(); ++i)
        {
            const sol::object obj = items[i];
            if (!obj.is<sol::table>())
            {
                entries.emplace_back(std::nullopt);
                continue;
            }

            const auto    entry = obj.as<sol::table>();
            entries.emplace_back(guildselllisthelpers::SourceEntry{
                .ItemNo = entry.get_or("id", static_cast<uint16>(0)),
                .Count  = entry.get_or("count", static_cast<uint8>(0)),
                .Max    = entry.get_or("max", static_cast<uint8>(0)),
                .Price  = entry.get_or("price", static_cast<int32>(0)),
            });
        }

        const auto plan = guildselllisthelpers::BuildResponsePlan(true, entries);
        PChar->pushPacket<GP_SERV_COMMAND_GUILD_SELLLIST>(PChar, plan.items);
    }
}
