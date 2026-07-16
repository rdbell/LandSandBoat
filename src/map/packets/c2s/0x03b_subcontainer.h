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
#include <vector>

#include "base.h"
#include "map/item_container.h"

enum class GP_CLI_COMMAND_SUBCONTAINER_KIND : uint32_t
{
    Equip      = 1,
    Unequip    = 2,
    UnequipAll = 5,
};

enum class GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX : uint32_t
{
    MainWeapon   = 0,
    SubWeapon    = 1,
    RangedWeapon = 2,
    Head         = 3,
    Body         = 4,
    Hands        = 5,
    Legs         = 6,
    Feet         = 7,
};

namespace subcontainerhelpers
{

enum class ItemLock : uint8
{
    Normal,
    Mannequin,
};

struct LockUpdate
{
    uint8_t  slot = 0;
    ItemLock lock = ItemLock::Normal;
};

struct Transition
{
    bool                  accepted = true;
    std::array<uint8, 8>  equipment{};
    std::vector<LockUpdate> lockUpdates{};
};

// BuildTransition mirrors SUBCONTAINER's mannequin equipment mutation. It
// leaves storage lookup, database persistence, and packet delivery to the map
// host. Replacing an occupied equipment slot deliberately does not unlock the
// former item, matching the native packet handler.
[[nodiscard]] inline auto BuildTransition(const GP_CLI_COMMAND_SUBCONTAINER_KIND kind, const CONTAINER_ID category2, const uint8 containerIndex,
                                          std::array<uint8, 8> equipment, const uint8 itemIndex2) -> Transition
{
    Transition transition{ .equipment = equipment };
    const uint8 equipmentIndex = containerIndex < transition.equipment.size() ? containerIndex : 0;
    const auto addLockUpdate = [&transition](const uint8 slot, const ItemLock lock) {
        if (slot != 0)
        {
            transition.lockUpdates.push_back({ slot, lock });
        }
    };

    switch (kind)
    {
        case GP_CLI_COMMAND_SUBCONTAINER_KIND::Equip:
            if (category2 != LOC_STORAGE)
            {
                transition.accepted = false;
                return transition;
            }

            if (transition.equipment[equipmentIndex] == itemIndex2)
            {
                addLockUpdate(itemIndex2, ItemLock::Normal);
                transition.equipment[equipmentIndex] = 0;
            }
            else
            {
                addLockUpdate(itemIndex2, ItemLock::Mannequin);
                transition.equipment[equipmentIndex] = itemIndex2;
            }
            break;
        case GP_CLI_COMMAND_SUBCONTAINER_KIND::Unequip:
            addLockUpdate(itemIndex2, ItemLock::Normal);
            transition.equipment[equipmentIndex] = 0;
            break;
        case GP_CLI_COMMAND_SUBCONTAINER_KIND::UnequipAll:
            for (auto& slot : transition.equipment)
            {
                addLockUpdate(slot, ItemLock::Normal);
                slot = 0;
            }
            break;
    }

    return transition;
}

} // namespace subcontainerhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x003B
// This packet is sent by the client when interacting with a sub-container item. (ie. Mannequins)
GP_CLI_PACKET(GP_CLI_COMMAND_SUBCONTAINER,
              uint32_t Kind;           // The mode of the packet.
              uint32_t Category1;      // The container that holds the sub-container item being interacted with.
              uint8_t  ItemIndex1;     // The index within the container that holds the sub-container item.
              uint8_t  ContainerIndex; // The index within the sub-container that is being interacted with.
              uint16_t padding00;      // Padding; unused.
              uint32_t Category2;      // The container that holds the item that will be used with the sub-container.
              uint8_t  ItemIndex2;     // The index within the container that holds the item.
              uint8_t  padding01[3];   // Padding; unused.
              uint32_t unknown00;      // Unknown
              uint8_t  unknown01;      // Unknown
              uint8_t  padding02[3];   // Padding; unused.
);
