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

enum class GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG : uint8_t
{
    Unequip       = 0,
    EquipOrCreate = 1,
};

enum class GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_LINKSHELLID : uint8_t
{
    Linkshell1 = 1,
    Linkshell2 = 2,
};

// GROUP_COMLINK_ACTIVE's first production routing decision. Item lookup is
// host-owned; all later item, linkshell, database, and packet effects remain
// in the map server helpers selected by this branch.
namespace groupcomlinkactive
{

enum class Branch : uint8_t
{
    None,
    Create,
    Equip,
    Unequip,
};

[[nodiscard]] inline auto BranchFor(const bool hasItem,
                                    const bool itemIsLinkshell,
                                    const bool itemIsNewLinkshell,
                                    const GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG activeFlag) -> Branch
{
    if (!hasItem || !itemIsLinkshell)
    {
        return Branch::None;
    }

    if (activeFlag == GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE_ACTIVEFLG::Unequip)
    {
        return Branch::Unequip;
    }

    if (itemIsNewLinkshell)
    {
        return Branch::Create;
    }

    return Branch::Equip;
}

} // namespace groupcomlinkactive

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00C4
// This packet is sent by the client when requesting to create a linkshell or when equipping/unequipping a linkshell item.
GP_CLI_PACKET(GP_CLI_COMMAND_GROUP_COMLINK_ACTIVE,
              uint16_t r : 4;            // PS2: r
              uint16_t g : 4;            // PS2: g
              uint16_t b : 4;            // PS2: b
              uint16_t a : 4;            // PS2: a
              uint8_t  ItemIndex;        // PS2: ItemIndex
              uint8_t  Category;         // PS2: (New; did not exist.)
              uint8_t  ActiveFlg;        // PS2: ActiveFlg
              uint8_t  padding00[3];     // PS2: (New; did not exist.)
              uint8_t  sComLinkName[15]; // PS2: sComLinkName
              uint8_t  LinkshellId;      // PS2: (New; did not exist.)
);
