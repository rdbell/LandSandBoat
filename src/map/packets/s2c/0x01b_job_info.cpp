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

#include "0x01b_job_info.h"
#include "job_info_runtime.h"

#include "aman.h"
#include "entities/char_entity.h"
#include "monstrosity.h"

GP_SERV_COMMAND_JOB_INFO::GP_SERV_COMMAND_JOB_INFO(CCharEntity* PChar)
{
    auto& packet = this->data();
    packet = jobinfohelpers::PlanFor({
        .race                = PChar->look.race,
        .mainJob             = static_cast<uint8>(PChar->GetMJob()),
        .subJob              = static_cast<uint8>(PChar->GetSJob()),
        .unlockedJobs        = PChar->jobs.unlocked,
        .jobLevels           = std::to_array(PChar->jobs.job),
        .baseStats           = std::to_array({ PChar->stats.STR, PChar->stats.DEX, PChar->stats.VIT, PChar->stats.AGI, PChar->stats.INT, PChar->stats.MND, PChar->stats.CHR }),
        .maxHP               = PChar->health.maxhp,
        .maxMP               = PChar->health.maxmp,
        .equipBlock          = PChar->m_EquipBlock,
        .statsDebilitation   = PChar->m_StatsDebilitation,
        .canThumbsUpMentor   = PChar->aman().canThumbsUp(),
        .mentorRank          = PChar->aman().getMentorRank(),
        .masteryRank         = PChar->aman().getMasteryRank(),
        .hasMonstrosity      = PChar->m_PMonstrosity != nullptr,
    });
}
