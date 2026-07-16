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

#include "0x059_friendpass.h"

auto friendpasshelpers::PlanFor(const uint32 worldPass) -> GP_SERV_COMMAND_FRIENDPASS::PacketData
{
    auto packet = GP_SERV_COMMAND_FRIENDPASS::PacketData{};

    // TODO: All of this is wrong
    packet.passPop   = 10000; // price
    packet.Type      = 0x03;
    packet.unknown21 = 0x01;

    if (worldPass != 0)
    {
        packet.leftNum  = 1;   // number of uses left
        packet.leftDays = 167; // pass becomes invalid in (hours)
        packet.Type     = 0x06;

        // Force to be 10 digits
        const std::string strbuff = fmt::format("{:0>10}", worldPass);

        std::memcpy(packet.String, strbuff.c_str(), strbuff.length());
    }

    return packet;
}

GP_SERV_COMMAND_FRIENDPASS::GP_SERV_COMMAND_FRIENDPASS(const uint32 worldPass)
{
    this->data() = friendpasshelpers::PlanFor(worldPass);
}
