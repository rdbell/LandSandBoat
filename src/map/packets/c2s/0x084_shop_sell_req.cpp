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

#include "0x084_shop_sell_req.h"

#include "entities/char_entity.h"
#include "packets/s2c/0x03d_shop_sell.h"
#include "trade_container.h"

// Go host pure half: packetsystem.ValidateShopSellReq / ProcessShopSellReq /
// NewShopSellReqHandler (6487); plan mappacket.ClientShopSellReqPlanFor.
auto GP_CLI_COMMAND_SHOP_SELL_REQ::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::Crafting });
}

void GP_CLI_COMMAND_SHOP_SELL_REQ::process(MapSession* PSession, CCharEntity* PChar) const
{
    const CItem* PItem = PChar->getStorage(LOC_INVENTORY)->GetItem(this->ItemIndex);
    const auto   plan  = shopsellreqhelpers::MakeAppraisalPlan(PItem != nullptr,
                                                               PItem && PItem->getID() == this->ItemNo,
                                                               PItem && PItem->hasFlag(ItemFlag::NoSale),
                                                               this->ItemNum,
                                                               PItem ? PItem->getQuantity() : 0);
    if (plan.appraise)
    {
        // Store item-to-sell in the last slot of the shop container
        PChar->Container->setItem(PChar->Container->getExSize(), this->ItemNo, this->ItemIndex, plan.quantity);
        PChar->pushPacket<GP_SERV_COMMAND_SHOP_SELL>(this->ItemIndex, PItem->getBasePrice());
    }
}
