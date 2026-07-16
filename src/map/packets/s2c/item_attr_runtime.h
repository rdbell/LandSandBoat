/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <algorithm>
#include <array>

#include "0x020_item_attr.h"
#include "enums/item_lockflg.h"

namespace itemattrhelpers
{

constexpr auto ExtraSize = 24;

struct Facts
{
    bool                    hasItem{};
    uint32                  quantity{};
    uint32                  price{};
    uint16                  itemID{};
    std::array<uint8, ExtraSize> extra{};
    bool                    charged{};
    uint8                   currentCharges{};
    uint8                   maxCharges{};
    bool                    reuseReady{};
    uint32                  nextUseTimestamp{};
    uint32                  delayTimestamp{};
    bool                    unlockableWeapon{};
    bool                    locked{};
    bool                    linkshell{};
    uint8                   linkshellType{};
    bool                    hasStaleItem{};
    std::array<uint8, ExtraSize> staleExtra{};
    bool                    leakStaleExtra{};
};

inline auto PutLE32(uint8* const target, const std::size_t offset, const uint32 value) -> void
{
    target[offset]     = static_cast<uint8>(value & 0xFF);
    target[offset + 1] = static_cast<uint8>((value >> 8) & 0xFF);
    target[offset + 2] = static_cast<uint8>((value >> 16) & 0xFF);
    target[offset + 3] = static_cast<uint8>((value >> 24) & 0xFF);
}

[[nodiscard]] inline auto PlanFor(const CONTAINER_ID location, const uint8 slot, const Facts& facts) -> GP_SERV_COMMAND_ITEM_ATTR::PacketData
{
    auto plan          = GP_SERV_COMMAND_ITEM_ATTR::PacketData{};
    plan.Category      = location;
    plan.ItemIndex     = slot;

    if (!facts.hasItem)
    {
        if (facts.hasStaleItem && facts.leakStaleExtra)
        {
            std::copy(facts.staleExtra.begin(), facts.staleExtra.end(), plan.Attr);
        }
        return plan;
    }

    plan.ItemNum = facts.quantity;
    plan.Price   = facts.price;
    plan.ItemNo  = facts.itemID;
    std::copy(facts.extra.begin(), facts.extra.end(), plan.Attr);

    if (facts.charged)
    {
        plan.Attr[0] = 0x01;
        uint8 flags  = 0x80;
        if (facts.currentCharges < facts.maxCharges)
        {
            flags |= 0x10;
        }
        if (facts.currentCharges > 0)
        {
            if (facts.reuseReady)
            {
                flags |= 0x40;
            }
            else
            {
                PutLE32(plan.Attr, 4, facts.nextUseTimestamp);
                PutLE32(plan.Attr, 8, facts.delayTimestamp);
            }
        }
        else
        {
            flags |= 0x20;
        }
        plan.Attr[3] = flags;
    }

    if (facts.unlockableWeapon)
    {
        plan.Attr[0] = 0;
        plan.Attr[1] = 0;
    }

    if (facts.price != 0)
    {
        plan.LockFlg = ItemLockFlg::Unknown0;
    }
    else if (facts.locked)
    {
        plan.LockFlg = facts.linkshell ? ItemLockFlg::Linkshell : ItemLockFlg::NoDrop;
    }
    else
    {
        plan.LockFlg = ItemLockFlg::Normal;
    }

    if (facts.linkshell)
    {
        plan.Attr[8] = facts.linkshellType;
    }
    return plan;
}

} // namespace itemattrhelpers
