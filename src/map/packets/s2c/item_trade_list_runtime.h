#pragma once

#include <algorithm>
#include <array>

#include "0x023_item_trade_list.h"

namespace itemtradelisthelpers
{

struct Facts
{
    uint32                reserve{};
    uint16                itemID{};
    bool                  charged{};
    uint8                 charges{};
    bool                  linkshell{};
    uint32                lsID{};
    uint16                lsColor{};
    uint8                 lsType{};
    std::array<uint8, 15> lsName{};
    std::array<uint8, 24> extra{};
};

[[nodiscard]] inline auto PlanFor(const uint8 slot, const Facts& facts) -> GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData
{
    auto packet       = GP_SERV_COMMAND_ITEM_TRADE_LIST::PacketData{};
    packet.ItemNum    = facts.reserve;
    packet.ItemNo     = facts.reserve == 0 ? 0 : facts.itemID;
    packet.TradeIndex = slot;

    if (facts.charged)
    {
        packet.Attr[0] = 1;
        if (facts.charges > 0)
        {
            packet.Attr[1] = facts.charges;
        }
    }
    else if (facts.linkshell)
    {
        for (std::size_t index = 0; index < 4; ++index)
        {
            packet.Attr[index] = static_cast<uint8>(facts.lsID >> (8 * index));
        }
        packet.Attr[6] = static_cast<uint8>(facts.lsColor);
        packet.Attr[7] = static_cast<uint8>(facts.lsColor >> 8);
        packet.Attr[8] = facts.lsType;
        std::copy(facts.lsName.begin(), facts.lsName.end(), packet.Attr + 9);
    }
    else
    {
        std::copy(facts.extra.begin(), facts.extra.end(), packet.Attr);
    }
    return packet;
}

} // namespace itemtradelisthelpers
