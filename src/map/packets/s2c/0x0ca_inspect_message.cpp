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

#include "0x0ca_inspect_message.h"

#include "entities/char_entity.h"

auto inspectmessageserverhelpers::PlanFor(const Facts& facts) -> GP_SERV_COMMAND_INSPECT_MESSAGE::PacketData
{
    auto packet = GP_SERV_COMMAND_INSPECT_MESSAGE::PacketData{};
    std::memcpy(packet.sInspectMessage, facts.bazaarMessage.data(), std::min(facts.bazaarMessage.size(), sizeof(packet.sInspectMessage)));
    packet.BazaarFlag    = 1;
    packet.MyFlag        = 1;
    packet.Race          = 1;
    packet.DesignationNo = facts.designationNo;
    std::memcpy(packet.sName, facts.name.data(), std::min(facts.name.size(), sizeof(packet.sName)));
    return packet;
}

GP_SERV_COMMAND_INSPECT_MESSAGE::GP_SERV_COMMAND_INSPECT_MESSAGE(const CCharEntity* PChar)
{
    this->data() = inspectmessageserverhelpers::PlanFor({
        .bazaarMessage = PChar->bazaar.message,
        .name          = PChar->getName(),
        .designationNo = PChar->profile.title,
    });
}
