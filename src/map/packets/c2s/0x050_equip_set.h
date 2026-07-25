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
#include <set>

#include "base.h"
#include "item_container.h"

namespace equipsethelpers
{

struct DispatchPlan
{
    bool equip            = true;
    bool persist          = true;
    bool checkGearSet     = true;
    bool updateHealth     = true;
    bool retriggerLatents = true;
};

struct ContainerFacts
{
    std::array<bool, 6> wardrobesAvailable{};
    bool                equipFromOtherContainers{};
    bool                mogSatchelAvailable{};
    bool                mogSackAvailable{};
    bool                mogCaseAvailable{};
};

constexpr auto MakeDispatchPlan() -> DispatchPlan
{
    return {};
}

auto ValidContainers(const ContainerFacts& facts) -> std::set<CONTAINER_ID>;

} // namespace equipsethelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0050
// This packet is sent by the client when changing equipment.
GP_CLI_PACKET(GP_CLI_COMMAND_EQUIP_SET,
              uint8_t PropertyItemIndex; // PS2: PropertyItemIndex
              uint8_t EquipKind;         // PS2: EquipKind
              uint8_t Category;          // PS2: (New; did not exist.)
);
