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

#include "0x05d_pendingstr.h"

#include <cstring>

auto pendingstrhelpers::PlanFor(const Facts& facts) -> GP_SERV_COMMAND_PENDINGSTR::PacketData
{
    auto packet = GP_SERV_COMMAND_PENDINGSTR::PacketData{};

    for (std::size_t index = 0; index < facts.params.size(); ++index)
    {
        packet.num[index] = static_cast<int32_t>(facts.params[index]);
    }

    std::memcpy(packet.string1, facts.strings[0].c_str(), std::min<size_t>(facts.strings[0].size(), sizeof(packet.string1) - 1));
    std::memcpy(packet.string2, facts.strings[1].c_str(), std::min<size_t>(facts.strings[1].size(), sizeof(packet.string2) - 1));
    std::memcpy(packet.string3, facts.strings[2].c_str(), std::min<size_t>(facts.strings[2].size(), sizeof(packet.string3) - 1));
    std::memcpy(packet.string4, facts.strings[3].c_str(), std::min<size_t>(facts.strings[3].size(), sizeof(packet.string4) - 1));

    return packet;
}

GP_SERV_COMMAND_PENDINGSTR::GP_SERV_COMMAND_PENDINGSTR(
    const std::string& string0,
    const std::string& string1,
    const std::string& string2,
    const std::string& string3,
    const uint32_t     param0,
    const uint32_t     param1,
    const uint32_t     param2,
    const uint32_t     param3,
    const uint32_t     param4,
    const uint32_t     param5,
    const uint32_t     param6,
    const uint32_t     param7,
    const uint32_t     param8)
{
    this->data() = pendingstrhelpers::PlanFor({
        .strings = { string0, string1, string2, string3 },
        .params  = { param0, param1, param2, param3, param4, param5, param6, param7, param8 },
    });
}
