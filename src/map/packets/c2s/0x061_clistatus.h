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

namespace clistatushelpers
{

// DispatchPlan identifies the live host operation chosen after CLISTATUS has
// passed packet validation. charutils owns the individual local-player packet
// bundle and its delivery order.
struct DispatchPlan
{
    bool refreshLocalPlayerPackets = false;
};

// MakeDispatchPlan mirrors GP_CLI_COMMAND_CLISTATUS::process. The process
// path is reached only after validate accepts unknown00 values 0 or 1; both
// values select the same local-player refresh.
auto MakeDispatchPlan() -> DispatchPlan;

} // namespace clistatushelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0061
// This packet is sent by the client when requesting the current clients local player information.
GP_CLI_PACKET(GP_CLI_COMMAND_CLISTATUS,
              uint8_t  unknown00; // PS2: (New; did not exist.)
              uint8_t  padding00; // PS2: (New; did not exist.)
              uint16_t padding01; // PS2: (New; did not exist.)
);
