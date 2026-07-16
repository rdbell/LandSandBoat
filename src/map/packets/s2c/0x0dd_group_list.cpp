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

#include "0x0dd_group_list.h"

#include "alliance.h"
#include "common/logging.h"
#include "entities/char_entity.h"
#include "entities/trust_entity.h"

GP_SERV_COMMAND_GROUP_LIST::GP_SERV_COMMAND_GROUP_LIST(const CCharEntity* PChar, const uint8_t MemberNumber, const uint16_t memberflags, const uint16_t ZoneID)
{
    if (PChar == nullptr)
    {
        ShowError("GP_SERV_COMMAND_GROUP_LIST::GP_SERV_COMMAND_GROUP_LIST() - PChar was null.");
        return;
    }

    auto&      packet = this->data();
    const auto plan   = grouplisthelpers::CharacterPlanFor({
        .common        = { .uniqueNo = PChar->id, .hp = PChar->health.hp, .mp = PChar->health.mp, .tp = PChar->health.tp, .actIndex = PChar->targid, .hpp = PChar->GetHPP(), .mpp = PChar->GetMPP(), .mjobNo = PChar->GetMJob(), .mjobLv = PChar->GetMLevel(), .sjobNo = PChar->GetSJob(), .sjobLv = PChar->GetSLevel(), .name = PChar->getName() },
        .memberFlags   = memberflags,
        .memberNumber  = MemberNumber,
        .entityZone    = PChar->getZone(),
        .requestedZone = ZoneID,
        .anonymous     = PChar->isAnon(),
    });
    packet            = plan.data;
    this->setSize(grouplisthelpers::PacketSizeForNameSize(plan.nameSize));
}

GP_SERV_COMMAND_GROUP_LIST::GP_SERV_COMMAND_GROUP_LIST(const CTrustEntity* PTrust, const uint8_t MemberNumber)
{
    if (PTrust == nullptr)
    {
        ShowError("GP_SERV_COMMAND_GROUP_LIST::GP_SERV_COMMAND_GROUP_LIST() - PTrust was null.");
        return;
    }

    auto&      packet = this->data();
    const auto plan   = grouplisthelpers::TrustPlanFor({
        .common       = { .uniqueNo = PTrust->id, .hp = PTrust->health.hp, .mp = PTrust->health.mp, .tp = PTrust->health.tp, .actIndex = PTrust->targid, .hpp = PTrust->GetHPP(), .mpp = PTrust->GetMPP(), .mjobNo = PTrust->GetMJob(), .mjobLv = PTrust->GetMLevel(), .sjobNo = PTrust->GetSJob(), .sjobLv = PTrust->GetSLevel(), .name = PTrust->getName() },
        .memberNumber = MemberNumber,
        .packetName   = PTrust->packetName,
    });
    packet            = plan.data;
    this->setSize(grouplisthelpers::PacketSizeForNameSize(plan.nameSize));
}

GP_SERV_COMMAND_GROUP_LIST::GP_SERV_COMMAND_GROUP_LIST(const uint32_t id, const std::string& name, const uint16_t memberFlags, const uint8_t MemberNumber, const uint16_t ZoneID)
{
    auto&      packet = this->data();
    const auto plan   = grouplisthelpers::DatabasePlanFor({ .uniqueNo = id, .name = name, .memberFlags = memberFlags, .zoneNo = ZoneID });
    packet            = plan.data;
    this->setSize(grouplisthelpers::PacketSizeForNameSize(plan.nameSize));
}
