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

#include "common/cbasetypes.h"

#include "base.h"

class CCharEntity;

struct record_t
{
    uint32_t Id : 12;
    uint32_t Count : 20;
};

namespace roeactiveloghelpers
{

constexpr std::size_t SourceRecordCount = 31;
constexpr std::size_t PacketRecordCount = 64;
constexpr std::size_t TimedRecordIndex  = 30;
constexpr std::size_t TimedPacketIndex  = 63;

struct Record
{
    uint32 id{};
    uint32 count{};
};

struct Facts
{
    std::array<Record, SourceRecordCount> records{};
};

struct Plan
{
    std::array<Record, PacketRecordCount> records{};
};

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> Plan
{
    auto plan = Plan{};
    for (std::size_t index = 0; index < TimedRecordIndex; ++index)
    {
        plan.records[index] = facts.records[index];
    }
    plan.records[TimedPacketIndex] = facts.records[TimedRecordIndex];
    return plan;
}

} // namespace roeactiveloghelpers

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x0111
// This packet is sent by the server to update the client's current set of Records of Eminence quest information.
class GP_SERV_COMMAND_ROE_ACTIVELOG final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_ROE_ACTIVELOG, GP_SERV_COMMAND_ROE_ACTIVELOG>
{
public:
    struct PacketData
    {
        record_t records[64];
    };

    GP_SERV_COMMAND_ROE_ACTIVELOG(const CCharEntity* PChar);
};
