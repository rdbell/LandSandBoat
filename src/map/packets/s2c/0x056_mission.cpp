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

#include "0x056_mission.h"

#include "entities/char_entity.h"
#include "enums/mission_log.h"

GP_SERV_COMMAND_MISSION::MISSION::MISSION(const CCharEntity* PChar)
{
    auto& packet = this->data();

    // Set packet.TalesBeginning.X to zero if declined
    auto declinedRoZStart = PChar->getCharVar("[ROZ]TalesBeginning");
    auto declinedCoPStart = PChar->getCharVar("[COP]TalesBeginning");
    auto declinedACPStart = PChar->getCharVar("[ACP]TalesBeginning");
    auto declinedAMKStart = PChar->getCharVar("[AMK]TalesBeginning");
    auto declinedASAStart = PChar->getCharVar("[ASA]TalesBeginning");
    auto declinedSoAStart = PChar->getCharVar("[SOA]TalesBeginning");
    auto declinedRoVStart = PChar->getCharVar("[ROV]TalesBeginning");

    const auto plan = missionhelpers::PlanFor({ PChar->profile.nation, PChar->m_missionLog[PChar->profile.nation].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::Zilart)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::CoP)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::ACP)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::AMK)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::ASA)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::SoA)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::RoV)].current, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::CoP)].statusLower, PChar->m_missionLog[static_cast<uint8_t>(MissionLog::CoP)].statusUpper, declinedRoZStart>0, declinedCoPStart>0, declinedACPStart>0, declinedAMKStart>0, declinedASAStart>0, declinedSoAStart>0, declinedRoVStart>0 });
    packet.Port             = plan.port;
    packet.Nation           = plan.nation;
    packet.NationMission    = plan.nationMission;
    packet.Expansion_RotZ   = plan.rotz;
    packet.Expansion_CoP    = plan.cop;
    packet.Expansion_CoP2   = plan.cop2;
    packet.Expansion_Addons = plan.addons;
    packet.Expansion_SoA    = plan.soa;
    packet.Expansion_RoV    = plan.rov;
    packet.TalesBeginning   = plan.tales;
}
