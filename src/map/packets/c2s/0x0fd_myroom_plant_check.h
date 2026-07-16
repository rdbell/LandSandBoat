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

#include <vector>

// Keeps gardening examine decisions testable without entities, packets, or
// persistence. The caller resolves flowerpot state and item IDs first.
namespace myroomplantcheck
{
enum class Message : uint8
{
    SeedSown,
    CrystalUsed,
    CrystalNone,
};

struct Output
{
    Message  message;
    uint16_t itemID;
};

struct Facts
{
    bool     isFlowerpot;
    bool     isPlanted;
    bool     isTree;
    bool     wasExamined;
    uint8_t  stage;
    uint16_t seedItemID;
    uint16_t extraCrystalItemID;
    uint16_t commonCrystalItemID;
};

struct Plan
{
    bool                sendMyRoomOperation;
    bool                markExamined;
    std::vector<Output> outputs;
};

auto PlanFor(const Facts& facts) -> Plan;
} // namespace myroomplantcheck

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00FD
// This packet is sent by the client when examining a plant within their mog house.
GP_CLI_PACKET(GP_CLI_COMMAND_MYROOM_PLANT_CHECK,
              uint16_t MyroomPlantItemNo;    // PS2: MyroomPlantItemNo
              uint8_t  MyroomPlantItemIndex; // PS2: MyroomPlantItemIndex
              uint8_t  MyroomPlantCategory;  // PS2: (New; did not exist.)
);
