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
#include "common/timer.h"
#include "enums/recast.h"

#include <cstddef>

class CCharEntity;

struct recasttimer_t
{
    uint16_t Timer;     // PS2: (New; did not exist.)
    uint8_t  Calc1;     // PS2: (New; did not exist.)
    uint8_t  TimerId;   // PS2: (New; did not exist.)
    uint16_t Calc2;     // PS2: (New; did not exist.)
    uint16_t padding06; // PS2: (New; did not exist.)
};

// The packet constructor resolves character recasts and charge catalog entries
// into these facts. Keeping the packet selection here makes its placement and
// timer arithmetic independently testable.
namespace abilrecasthelpers
{

struct RecastFact
{
    Recast            id{};
    timer::time_point timestamp{};
    timer::duration   recastTime{};
    timer::duration   chargeTime{};
    uint8             maxCharges{};
    bool              hasBaseCharge{};
    timer::duration   baseChargeTime{};
};

struct Plan
{
    recasttimer_t timers[31]{};
    uint32        mountRecast{};
    uint32        mountRecastId{};
};

// Apply appends one native ability recast fact to plan. normalCount must start
// at one because slot zero is reserved for the special ability recast. It
// returns true when the native constructor must reject further normal entries.
[[nodiscard]] auto Apply(Plan& plan, uint8& normalCount, const RecastFact& recast, timer::time_point now) -> bool;

} // namespace abilrecasthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/server/0x0119
// This packet is sent by the server to update the clients ability (and mount) recast information.
class GP_SERV_COMMAND_ABIL_RECAST final : public GP_SERV_PACKET<PacketS2C::GP_SERV_COMMAND_ABIL_RECAST, GP_SERV_COMMAND_ABIL_RECAST>
{
public:
    struct PacketData
    {
        recasttimer_t Timers[31];    // PS2: (New; did not exist.)
        uint32_t      MountRecast;   // PS2: (New; did not exist.)
        uint32_t      MountRecastId; // PS2: (New; did not exist.)
    };

    GP_SERV_COMMAND_ABIL_RECAST(CCharEntity* PChar);
};
