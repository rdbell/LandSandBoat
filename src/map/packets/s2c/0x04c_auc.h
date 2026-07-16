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
#include "packets/c2s/0x04e_auc.h"

enum class GP_CLI_COMMAND_AUC_COMMAND : uint8_t;
class CCharEntity;
class CItem;

// AUC AskCommit derives a posting fee from host-provided settings, then fills
// a fixed subset of the response. Item and settings lookups remain host work.
namespace auchelpters
{

constexpr uint32 AuctionHouseId = 4; // Jeuno linked AH

struct AskCommitFacts
{
    uint8  command{};
    uint16 itemId{};
    uint16 itemWorkIndex{};
    uint8  itemStacks{};
    uint32 price{};
    uint32 stackBaseFee{};
    float  stackTaxRate{};
    uint32 singleBaseFee{};
    float  singleTaxRate{};
    uint32 maxFee{};
};

struct AskCommitPlan
{
    uint8  command{};
    int8   aucWorkIndex{-1};
    int8   result{1};
    int8   resultStatus{2};
    uint32 commission{};
    uint16 itemId{};
    uint16 itemWorkIndex{};
    uint8  itemStacks{};
    uint32 marketNo{AuctionHouseId};
};

[[nodiscard]] inline auto AskCommitPlanFor(const AskCommitFacts facts) -> AskCommitPlan
{
    const auto baseFee = facts.itemStacks == 0 ? facts.stackBaseFee : facts.singleBaseFee;
    const auto taxRate = facts.itemStacks == 0 ? facts.stackTaxRate : facts.singleTaxRate;
    const auto fee     = static_cast<uint32>(static_cast<float>(baseFee) + (static_cast<float>(facts.price) * taxRate / 100.0F));

    const auto commission = fee > facts.maxFee ? facts.maxFee : fee;

    return {
        facts.command,
        -1,
        1,
        2,
        commission,
        facts.itemId,
        facts.itemWorkIndex,
        facts.itemStacks,
        AuctionHouseId,
    };
}

} // namespace auchelpters

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x004C
// This packet is sent by the server when the client is interacting with the auction house.
class GP_SERV_COMMAND_AUC final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_AUC, GP_SERV_COMMAND_AUC>
{
public:
    struct PacketData
    {
        GP_CLI_COMMAND_AUC_COMMAND Command;      // PS2: Command
        int8_t                     AucWorkIndex; // PS2: AucWorkIndex
        char                       Result;       // PS2: Result
        char                       ResultStatus; // PS2: ResultStatus
        GP_AUC_PARAM               Param;        // PS2: Param
        GP_AUC_BOX                 Parcel;       // PS2: Parcel
    };

    GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND action);                                       // Send the auction menu
    GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND action, uint8 slot, const CCharEntity* PChar); // Send the list of items sold by a character
    GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND action, uint8 message, uint16 itemid, uint32 price, uint8 quantity, uint8 stacksize);
    GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND action, uint8 message, const CCharEntity* PChar, uint8 slot, bool keepItem);
    GP_SERV_COMMAND_AUC(GP_CLI_COMMAND_AUC_COMMAND action, const CItem* PItem, uint8 quantity, uint32 price);
};
