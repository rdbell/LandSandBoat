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

namespace extendedjobmonhelpers
{

constexpr uint8 JobMon = 23;

struct Facts
{
    bool                   hasMonstrosity{};
    uint16                 species{};
    std::array<uint16, 12> equippedInstincts{};
};

struct Plan
{
    uint8                  job{ JobMon };
    uint16                 species{};
    std::array<uint16, 12> equippedInstincts{};
};

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> Plan
{
    auto plan = Plan{};
    if (facts.hasMonstrosity)
    {
        plan.species           = facts.species;
        plan.equippedInstincts = facts.equippedInstincts;
    }
    return plan;
}

} // namespace extendedjobmonhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x0044
// This packet is sent by the server to populate the clients extended job information.
namespace GP_SERV_COMMAND_EXTENDED_JOB
{

class MON final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_EXTENDED_JOB, MON>
{
public:
    struct PacketData
    {
        uint8_t  Job;      // PS2: Job
        uint8_t  IsSubJob; // PS2: IsSubJob
        uint8_t  padding01[2];
        uint16_t Species;
        uint8_t  padding02[2];
        uint16_t EquippedInstincts[12];
        uint8_t  padding03[124];
    };

    MON(const CCharEntity* PChar);
};

} // namespace GP_SERV_COMMAND_EXTENDED_JOB
