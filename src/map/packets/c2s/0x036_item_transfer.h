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

#include <array>

#include "base.h"

namespace itemtransferhelpers
{

enum class Action : uint8
{
    CannotWhileInvisible,
    NoOp,
    InvalidItem,
    ReservedItem,
    LockedItem,
    BeginTrade,
};

struct ItemFacts
{
    bool validQuantity;
    bool reserved;
    bool locked;
};

struct RuntimeFacts
{
    bool                    invisible;
    bool                    entityResolved;
    bool                    entityIdMatches;
    float                   entityDistance;
    bool                    entityIsMob;
    bool                    mobStatusNormal;
    std::array<ItemFacts, 9> items{};
    uint8                   itemCount;
};

struct Plan
{
    Action action;
    bool   sendCannotWhileInvisible;
    bool   cleanTrade;
    uint8  reservedItemCount;
    bool   invokeOnTrade;
    bool   unreserveUnconfirmed;
};

// MakePlan mirrors process's host-independent branch ordering. In particular,
// it retains reservations made before a later bad item causes an early return.
// Entity/storage access, auditing, trade-container mutation, Lua dispatch, and
// event/crafting side effects remain map-host responsibilities.
[[nodiscard]] constexpr auto MakePlan(const RuntimeFacts& facts) -> Plan
{
    if (facts.invisible)
    {
        return { Action::CannotWhileInvisible, true, false, 0, false, false };
    }

    if (!facts.entityResolved || !facts.entityIdMatches || facts.entityDistance > 6.0f ||
        (facts.entityIsMob && !facts.mobStatusNormal))
    {
        return { Action::NoOp, false, false, 0, false, false };
    }

    auto plan = Plan{ Action::BeginTrade, false, true, 0, false, false };
    for (uint8 slotId = 0; slotId < facts.itemCount; ++slotId)
    {
        if (slotId >= facts.items.size())
        {
            plan.action = Action::InvalidItem;
            return plan;
        }
        const auto& item = facts.items[slotId];
        if (!item.validQuantity)
        {
            plan.action = Action::InvalidItem;
            return plan;
        }
        if (item.reserved)
        {
            plan.action = Action::ReservedItem;
            return plan;
        }
        if (item.locked)
        {
            plan.action = Action::LockedItem;
            return plan;
        }

        ++plan.reservedItemCount;
    }

    plan.invokeOnTrade         = true;
    plan.unreserveUnconfirmed = true;
    return plan;
}

// ShouldForceSynthCriticalFail mirrors ITEM_TRANSFER's post-OnTrade gate.
// Lua may start an event while an earlier synthesis remains active, so this
// must be evaluated after OnTrade and unreserveUnconfirmed.
[[nodiscard]] constexpr auto ShouldForceSynthCriticalFail(const bool inEvent, const bool crafting) -> bool
{
    return inEvent && crafting;
}

} // namespace itemtransferhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0036
// This packet is sent by the client when completing a trade with an NPC.
GP_CLI_PACKET(GP_CLI_COMMAND_ITEM_TRANSFER,
              uint32_t UniqueNo;                 // PS2: UniqueNo
              uint32_t ItemNumTbl[10];           // PS2: ItemNumTbl
              uint8_t  PropertyItemIndexTbl[10]; // PS2: PropertyItemIndexTbl
              uint16_t ActIndex;                 // PS2: ActIndex
              uint8_t  ItemNum;                  // PS2: ItemNum
              uint8_t  padding00[3];             // PS2: padding00
);
