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

#include "0x076_group_effects.h"

#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "status_effect_container.h"

GP_SERV_COMMAND_GROUP_EFFECTS::GP_SERV_COMMAND_GROUP_EFFECTS(const std::vector<CCharEntity*>& membersList)
{
    auto& packet = this->data();

    auto facts = std::vector<groupeffectshelpers::MemberFacts>{};
    facts.reserve(groupeffectshelpers::MemberCount);
    for (std::size_t index = 0; index < membersList.size() && index < groupeffectshelpers::MemberCount; ++index)
    {
        const auto* member = membersList[index];
        auto        fact   = groupeffectshelpers::MemberFacts{};
        fact.uniqueNo      = member->id;
        fact.actIndex      = member->targid;
        fact.statusBits    = member->StatusEffectContainer->statusBits();
        std::memcpy(fact.buffs.data(), member->StatusEffectContainer->statusIcons(), fact.buffs.size());
        facts.push_back(fact);
    }
    const auto plan = groupeffectshelpers::PlanFor(facts);

    for (std::size_t idx = 0; idx < plan.members.size(); ++idx)
    {
        packet.Members[idx].UniqueNo = plan.members[idx].uniqueNo;
        packet.Members[idx].ActIndex = plan.members[idx].actIndex;
        packet.Members[idx].Bits     = plan.members[idx].statusBits;
        std::memcpy(packet.Members[idx].Buffs, plan.members[idx].buffs.data(), plan.members[idx].buffs.size());
    }
}
