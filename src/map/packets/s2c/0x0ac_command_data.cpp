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

#include "0x0ac_command_data.h"

#include <cstring>

#include "entities/char_entity.h"

auto commanddatahelpers::PlanFor(const Facts& facts) -> GP_SERV_COMMAND_COMMAND_DATA::PacketData
{
    auto packet = GP_SERV_COMMAND_COMMAND_DATA::PacketData{};
    std::memcpy(packet.CommandDataTbl.WeaponSkills, facts.weaponSkills.data(), facts.weaponSkills.size());
    std::memcpy(packet.CommandDataTbl.JobAbilities, facts.jobAbilities.data(), facts.jobAbilities.size());
    std::memcpy(packet.CommandDataTbl.PetAbilities, facts.petAbilities.data(), facts.petAbilities.size());
    std::memcpy(packet.CommandDataTbl.Traits, facts.traits.data(), facts.traits.size());
    return packet;
}

GP_SERV_COMMAND_COMMAND_DATA::GP_SERV_COMMAND_COMMAND_DATA(const CCharEntity* PChar)
{
    auto facts = commanddatahelpers::Facts{};
    std::memcpy(facts.weaponSkills.data(), PChar->m_WeaponSkills, sizeof(PChar->m_WeaponSkills));
    std::memcpy(facts.jobAbilities.data(), PChar->m_Abilities, sizeof(PChar->m_Abilities));
    std::memcpy(facts.petAbilities.data(), PChar->m_PetCommands, sizeof(PChar->m_PetCommands));
    std::memcpy(facts.traits.data(), PChar->m_TraitList, sizeof(PChar->m_TraitList));
    this->data() = commanddatahelpers::PlanFor(facts);
}
