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

#include "common/cbasetypes.h"

#include "base.h"

#include <array>

class CItem;

namespace bazaarlisthelpers
{

struct ItemFacts
{
    bool                  present{};
    uint32                price{};
    uint32                quantity{};
    uint16                itemId{};
    bool                  chargedUsable{};
    uint8                 charges{};
    bool                  nextUseFuture{};
    uint32                nextUseTimestamp{};
    uint32                delayTimestamp{};
    std::array<uint8, 24> extra{};
};

struct Plan
{
    uint32                price{};
    uint32                quantity{};
    uint16                tax{};
    uint16                itemId{};
    uint8                 slot{};
    std::array<uint8, 24> attr{};
};

[[nodiscard]] constexpr auto PlanFor(const uint8 slot, const uint16 tax, const ItemFacts& item) -> Plan
{
    Plan plan{ .slot = slot };
    if (!item.present)
    {
        return plan;
    }
    plan.price    = item.price;
    plan.quantity = item.quantity;
    plan.tax      = tax;
    plan.itemId   = item.itemId;
    if (!item.chargedUsable)
    {
        plan.attr = item.extra;
        return plan;
    }
    plan.attr[0]  = 0x01;
    plan.attr[1]  = item.charges;
    plan.attr[3]  = item.nextUseFuture ? 0x90 : 0xD0;
    plan.attr[4]  = static_cast<uint8>(item.nextUseTimestamp);
    plan.attr[5]  = static_cast<uint8>(item.nextUseTimestamp >> 8);
    plan.attr[6]  = static_cast<uint8>(item.nextUseTimestamp >> 16);
    plan.attr[7]  = static_cast<uint8>(item.nextUseTimestamp >> 24);
    plan.attr[8]  = static_cast<uint8>(item.delayTimestamp);
    plan.attr[9]  = static_cast<uint8>(item.delayTimestamp >> 8);
    plan.attr[10] = static_cast<uint8>(item.delayTimestamp >> 16);
    plan.attr[11] = static_cast<uint8>(item.delayTimestamp >> 24);
    return plan;
}

} // namespace bazaarlisthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x0105
// This packet is sent by the server when the client is checking another players Bazaar. This packet is used to populate the checked players bazaar items.
class GP_SERV_COMMAND_BAZAAR_LIST final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_BAZAAR_LIST, GP_SERV_COMMAND_BAZAAR_LIST>
{
public:
    struct PacketData
    {
        uint32_t Price;        // PS2: Price
        uint32_t ItemNum;      // PS2: ItemNum
        uint16_t TaxRate;      // PS2: TaxRate
        uint16_t ItemNo;       // PS2: ItemNo
        uint8_t  ItemIndex;    // PS2: ItemIndex
        uint8_t  Attr[24];     // PS2: Attr
        uint8_t  padding00[3]; // PS2: (New; did not exist.))
    };

    GP_SERV_COMMAND_BAZAAR_LIST(CItem* PItem, uint8 slotId, uint16 tax);
};
