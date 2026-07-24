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

#include "0x0ac_guild_sell.h"

#include "common/database.h"
#include "common/settings.h"
#include "entities/char_entity.h"
#include "items/item_shop.h"
#include "lua/luautils.h"
#include "packets/s2c/0x01d_item_same.h"
#include "packets/s2c/0x084_guild_sell.h"
#include "utils/charutils.h"
#include "utils/itemutils.h"
#include "utils/zoneutils.h"

namespace
{

const auto auditSale = [](Scheduler& scheduler, CCharEntity* PChar, uint32_t itemId, uint32_t basePrice, uint8_t quantity)
{
    if (settings::get<bool>("map.AUDIT_PLAYER_VENDOR"))
    {
        scheduler.postToWorkerThread(
            [itemId, quantity, seller = PChar->id, sellerName = PChar->getName(), basePrice]()
            {
                auto totalPrice = basePrice * quantity;

                const auto query = "INSERT INTO audit_vendor(itemid, quantity, seller, seller_name, baseprice, totalprice, date) VALUES (?, ?, ?, ?, ?, ?, UNIX_TIMESTAMP())";
                if (!db::preparedStmt(query, itemId, quantity, seller, sellerName, basePrice, totalPrice))
                {
                    ShowErrorFmt("Failed to log vendor sale (item: {}, quantity: {}, seller: {}, baseprice: {}, totalprice: {})",
                                 itemId,
                                 quantity,
                                 seller,
                                 basePrice,
                                 totalPrice);
                }
            });
    }
};

} // namespace

// Go host pure half: packetsystem.ValidateGuildSell / ProcessGuildSell /
// NewGuildSellHandler (6494); plan mappacket.ClientGuildSellPlanFor.
auto GP_CLI_COMMAND_GUILD_SELL::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::Crafting })
        .mustNotEqual(PChar->guildShopNpc_.id, 0, "Character does not have a guild shop")
        .range("ItemNum", this->ItemNum, 1, 99);
}

void GP_CLI_COMMAND_GUILD_SELL::process(MapSession* PSession, CCharEntity* PChar) const
{
    const CItem* PItem = xi::items::lookup(this->ItemNo);
    if (!PItem)
    {
        ShowWarning("User '%s' attempting to sell an invalid item to guild vendor!", PChar->getName());
        return;
    }

    const auto preScriptPlan = guildsellhelpers::MakePlan(this->ItemNum, {
        .itemFound = true,
        .itemStackSize = PItem->getStackSize(),
    });

    // A guild shop never buys more than a single stack of an item per transaction.
    if (preScriptPlan.action == guildsellhelpers::Action::RejectOverStack)
    {
        PChar->pushPacket<GP_SERV_COMMAND_GUILD_SELL>(PChar, preScriptPlan.stock, preScriptPlan.itemNo, preScriptPlan.trade);
        return;
    }

    if (auto* PNpc = zoneutils::GetEntity(PChar->guildShopNpc_.id, TYPE_NPC))
    {
        const auto result = luautils::callGlobal<sol::table>("xi.guildShops.onPlayerSell", PChar, PNpc, this->ItemNo, this->ItemNum);
        if (result.valid())
        {
            const auto itemNo = result.get_or("itemNo", uint16{ 0 });
            const auto count  = result.get_or("count", uint8{ 0 });
            const auto trade  = result.get_or("trade", int32{ 0 });
            const auto sold   = result.get_or("sold", uint8{ 0 });
            const auto price  = result.get_or("price", uint32{ 0 });
            const auto plan = guildsellhelpers::MakePlan(this->ItemNum, {
                .itemFound = true,
                .itemStackSize = PItem->getStackSize(),
                .guildShopNpcFound = true,
                .scriptResultValid = true,
                .auditPlayerVendor = settings::get<bool>("map.AUDIT_PLAYER_VENDOR"),
                .scriptItemNo = itemNo,
                .scriptCount = count,
                .scriptTrade = trade,
                .scriptSold = sold,
                .scriptPrice = price,
            });
            PChar->pushPacket<GP_SERV_COMMAND_GUILD_SELL>(PChar, plan.stock, plan.itemNo, plan.trade);

            if (plan.auditSale)
            {
                auditSale(*PSession->scheduler, PChar, plan.auditItemNo, plan.auditBasePrice, plan.auditQuantity);
            }
        }
    }
}
