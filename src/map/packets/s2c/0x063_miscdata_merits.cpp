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

#include "0x063_miscdata_merits.h"

#include "entities/char_entity.h"
#include "enums/key_items.h"
#include "job_points.h"
#include "merit.h"
#include "utils/charutils.h"

GP_SERV_COMMAND_MISCDATA::MERITS::MERITS(CCharEntity* PChar)
{
    auto& packet = this->data();

    packet.type      = GP_SERV_COMMAND_MISCDATA_TYPE::Merits;
    packet.unknown06 = sizeof(PacketData);

    const auto mainJob      = PChar->GetMJob();
    const auto mainJobLevel = PChar->GetMLevel();
    const auto plan = miscdatameritshelpers::PlanFor({
        .limitPoints                = PChar->PMeritPoints->GetLimitPoints(),
        .meritPoints                = PChar->PMeritPoints->GetMeritPoints(),
        .mainJobIsBlueMage          = mainJob == JOB_BLU,
        .mainJobLevel               = mainJobLevel,
        .hasLimitBreaker            = charutils::hasKeyItem(PChar, KeyItem::LIMIT_BREAKER),
        .meritMode                  = PChar->MeritMode,
        .blueAssimilationBonus      = mainJob == JOB_BLU && mainJobLevel >= 75 ? PChar->PMeritPoints->GetMeritValue(MERIT_ASSIMILATION, PChar) : 0,
        .blueJobPointBonus          = mainJob == JOB_BLU && mainJobLevel >= 99 ? PChar->PJobPoints->GetJobPointValue(JP_BLUE_MAGIC_POINT_BONUS) : 0,
        .currentJobLevel            = PChar->jobs.job[mainJob],
        .levelLimit                 = PChar->jobs.genkai,
        .currentExperience          = PChar->jobs.exp[mainJob],
        .experienceForNextLevel     = charutils::GetExpNEXTLevel(PChar->jobs.job[mainJob]),
        .configuredMaxMeritPoints   = settings::get<uint8>("map.MAX_MERIT_POINTS"),
        .maxMeritBonus              = PChar->PMeritPoints->GetMeritValue(MERIT_MAX_MERIT, PChar),
    });

    packet.limitPoints          = plan.limitPoints;
    packet.meritPoints          = plan.meritPoints;
    packet.bluBonus             = plan.bluBonus;
    packet.canUseMeritMode      = plan.canUseMeritMode;
    packet.xpCappedOrMeritMode  = plan.xpCappedOrMeritMode;
    packet.meritModeEnabled     = plan.meritModeEnabled;
    packet.maxMeritPoints       = plan.maxMeritPoints;
}
