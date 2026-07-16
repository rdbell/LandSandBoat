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

#include "0x0df_group_attr.h"

#include "entities/char_entity.h"
#include "entities/trust_entity.h"
#include "monstrosity.h"

GP_SERV_COMMAND_GROUP_ATTR::GP_SERV_COMMAND_GROUP_ATTR(CCharEntity* PChar)
{
    auto&      packet = this->data();
    const auto plan   = groupattrhelpers::CharacterPlanFor({
        .common = {
            .uniqueNo = PChar->id,
            .hp       = static_cast<uint32>(PChar->health.hp),
            .mp       = static_cast<uint32>(PChar->health.mp),
            .tp       = static_cast<uint32>(PChar->health.tp),
            .actIndex = PChar->targid,
            .hpp      = PChar->GetHPP(),
            .mpp      = PChar->GetMPP(),
            .mjobNo   = PChar->GetMJob(),
            .mjobLv   = PChar->GetMLevel(),
            .sjobNo   = PChar->GetSJob(),
            .sjobLv   = PChar->GetSLevel(),
        },
        .anonymous         = PChar->isAnon(),
        .hasMonstrosity    = PChar->m_PMonstrosity != nullptr,
        .monstrosityNameId = PChar->m_PMonstrosity != nullptr ? static_cast<uint16>(monstrosity::GetPackedMonstrosityName(PChar)) : uint16{},
    });

    packet.UniqueNo          = plan.uniqueNo;
    packet.Hp                = plan.hp;
    packet.Mp                = plan.mp;
    packet.Tp                = plan.tp;
    packet.ActIndex          = plan.actIndex;
    packet.Hpp               = plan.hpp;
    packet.Mpp               = plan.mpp;
    packet.MonstrosityNameId = plan.monstrosityNameId;
    packet.mjob_no           = plan.mjobNo;
    packet.mjob_lv           = plan.mjobLv;
    packet.sjob_no           = plan.sjobNo;
    packet.sjob_lv           = plan.sjobLv;
}

GP_SERV_COMMAND_GROUP_ATTR::GP_SERV_COMMAND_GROUP_ATTR(CTrustEntity* PTrust)
{
    auto& packet = this->data();

    const auto plan = groupattrhelpers::TrustPlanFor({
        .uniqueNo = PTrust->id,
        .hp       = static_cast<uint32>(PTrust->health.hp),
        .mp       = static_cast<uint32>(PTrust->health.mp),
        .tp       = static_cast<uint32>(PTrust->health.tp),
        .actIndex = PTrust->targid,
        .hpp      = PTrust->GetHPP(),
        .mpp      = PTrust->GetMPP(),
        .mjobNo   = PTrust->GetMJob(),
        .mjobLv   = PTrust->GetMLevel(),
        .sjobNo   = PTrust->GetSJob(),
        .sjobLv   = PTrust->GetSLevel(),
    });

    packet.UniqueNo = plan.uniqueNo;
    packet.Hp       = plan.hp;
    packet.Mp       = plan.mp;
    packet.Tp       = plan.tp;
    packet.ActIndex = plan.actIndex;
    packet.Hpp      = plan.hpp;
    packet.Mpp      = plan.mpp;
    packet.mjob_no  = plan.mjobNo;
    packet.mjob_lv  = plan.mjobLv;
    packet.sjob_no  = plan.sjobNo;
    packet.sjob_lv  = plan.sjobLv;
}
