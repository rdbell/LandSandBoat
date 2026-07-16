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

namespace itemusehelpers
{

enum class Action : uint8
{
    NoOp,
    TooFarAway,
    InvalidItem,
    BeginUse,
    UnableToUseItem,
};

struct RuntimeFacts
{
    bool  targetResolved;
    float targetDistance;
    bool  itemResolved;
    bool  itemIsEquipment;
    bool  itemLocked;
    bool  itemEquipped;
    bool  itemReserved;
    bool  itemBazaared;
    bool  useItemContainerActive;
};

// DecideAction mirrors the host-independent branch ordering in process.
// Entity lookup, distance calculation, storage access, and action dispatch
// remain map-host responsibilities.
[[nodiscard]] constexpr auto DecideAction(const RuntimeFacts& facts) -> Action
{
    if (!facts.targetResolved)
    {
        return Action::NoOp;
    }

    if (facts.targetDistance > 12.0f)
    {
        return Action::TooFarAway;
    }

    if (!facts.itemResolved)
    {
        return Action::NoOp;
    }

    const bool itemLocked = facts.itemLocked && !(facts.itemIsEquipment && facts.itemEquipped);
    if (itemLocked || facts.itemReserved || facts.itemBazaared)
    {
        return Action::InvalidItem;
    }

    return facts.useItemContainerActive ? Action::UnableToUseItem : Action::BeginUse;
}

} // namespace itemusehelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0037
// This packet is sent by the client when using an item.
GP_CLI_PACKET(GP_CLI_COMMAND_ITEM_USE,
              uint32_t UniqueNo;          // PS2: UniqueNo
              uint32_t ItemNum;           // PS2: ItemNum
              uint16_t ActIndex;          // PS2: ActIndex
              uint8_t  PropertyItemIndex; // PS2: PropertyItemIndex
              uint8_t  padding00;         // PS2: dammy2
              uint32_t Category;          // PS2: (New; did not exist.)
);
