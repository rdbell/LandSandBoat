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

#include "0x04c_auc.h"

#include "entities/char_entity.h"
#include "packets/c2s/0x04e_auc.h"

GP_SERV_COMMAND_AUC::GP_SERV_COMMAND_AUC(const GP_CLI_COMMAND_AUC_COMMAND action)
{
    auto& packet = this->data();

    packet.Command      = action;
    packet.AucWorkIndex = -1;
    packet.Result       = 1; // Auction House open
}

GP_SERV_COMMAND_AUC::GP_SERV_COMMAND_AUC(const GP_CLI_COMMAND_AUC_COMMAND action, const CItem* PItem, const uint8 quantity, const uint32 price)
{
    auto& packet = this->data();

    const auto plan = auchelpters::AskCommitPlanFor({
        .command       = static_cast<uint8>(action),
        .itemId        = PItem->getID(),
        .itemWorkIndex = PItem->getSlotID(),
        .itemStacks    = quantity,
        .price         = price,
        .stackBaseFee  = settings::get<uint32>("map.AH_BASE_FEE_STACKS"),
        .stackTaxRate  = settings::get<float>("map.AH_TAX_RATE_STACKS"),
        .singleBaseFee = settings::get<uint32>("map.AH_BASE_FEE_SINGLE"),
        .singleTaxRate = settings::get<float>("map.AH_TAX_RATE_SINGLE"),
        .maxFee        = settings::get<uint32>("map.AH_MAX_FEE"),
    });

    packet.Command                       = static_cast<GP_CLI_COMMAND_AUC_COMMAND>(plan.command);
    packet.AucWorkIndex                  = plan.aucWorkIndex;
    packet.Result                        = plan.result;
    packet.ResultStatus                  = plan.resultStatus;
    packet.Param.AskCommit.Commission    = plan.commission;
    packet.Param.AskCommit.ItemNo        = plan.itemId;
    packet.Param.AskCommit.ItemWorkIndex = plan.itemWorkIndex;
    packet.Param.AskCommit.ItemStacks    = plan.itemStacks;
    packet.Parcel.MarketNo               = plan.marketNo;
}

// e.g. client history, client probes a slot number which you give the correct itemId+price
GP_SERV_COMMAND_AUC::GP_SERV_COMMAND_AUC(const GP_CLI_COMMAND_AUC_COMMAND action, const uint8 slot, const CCharEntity* PChar)
{
    auto& packet = this->data();

    packet.Command      = action;
    packet.AucWorkIndex = slot;
    packet.Result       = 1;

    if (slot < 7 && slot < PChar->m_ah_history.size())
    {
        packet.Parcel.Stat         = 0x03;
        packet.Parcel.ItemIndex    = 0x01;                                   // Value is changed, the purpose is unknown
        packet.Parcel.ItemNo       = PChar->m_ah_history.at(slot).itemid;    // Item ID of item in slot
        packet.Parcel.ItemQuantity = 1 - PChar->m_ah_history.at(slot).stack; // Number of items stack size
        packet.Parcel.ItemCategory = 0x02;                                   // Number of items stack size?
        packet.Parcel.Price        = PChar->m_ah_history.at(slot).price;     // Selling price
        packet.Parcel.MarketNo     = auchelpters::AuctionHouseId;
    }
}

GP_SERV_COMMAND_AUC::GP_SERV_COMMAND_AUC(const GP_CLI_COMMAND_AUC_COMMAND action, const uint8 message, const uint16 itemid, const uint32 price, const uint8 quantity, const uint8 stacksize)
{
    auto& packet = this->data();

    packet.Command            = action;
    packet.Result             = message;
    packet.Param.Bid.BidPrice = price;
    packet.Param.Bid.ItemNo   = itemid;
    // 0 = stack, 1 = single
    packet.Param.Bid.ItemStacks = quantity == 0 ? stacksize : 1;
}

GP_SERV_COMMAND_AUC::GP_SERV_COMMAND_AUC(const GP_CLI_COMMAND_AUC_COMMAND action, const uint8 message, const CCharEntity* PChar, const uint8 slot, const bool keepItem)
{
    auto& packet = this->data();

    packet.Command      = action;
    packet.AucWorkIndex = slot;
    packet.Result       = message;

    // we need all this guff so the item stays in the history.
    if (keepItem && slot < 7 && slot < PChar->m_ah_history.size())
    {
        packet.Parcel.Stat         = 0x03;
        packet.Parcel.ItemIndex    = 0x01;                                   // Value is changed, the purpose is unknown
        packet.Parcel.ItemNo       = PChar->m_ah_history.at(slot).itemid;    // Id sell items item id
        packet.Parcel.ItemQuantity = 1 - PChar->m_ah_history.at(slot).stack; // Number of items stack size
        packet.Parcel.ItemCategory = 0x02;                                   // Number of items stack size?
        packet.Parcel.Price        = PChar->m_ah_history.at(slot).price;     // Price selling price
        packet.Parcel.MarketNo     = auchelpters::AuctionHouseId;

        std::memcpy(packet.Parcel.Name, PChar->getName().c_str(), std::min(PChar->getName().size(), sizeof(packet.Parcel.Name)));
    }
}
