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

#include "0x119_abil_recast.h"

#include "common/timer.h"

#include <cstring>

#include "ability.h"
#include "entities/char_entity.h"
#include "recast_container.h"

auto abilrecasthelpers::Apply(Plan& plan, uint8& normalCount, const RecastFact& recast, const timer::time_point now) -> bool
{
    const auto remaining     = recast.recastTime == 0s ? 0s : std::chrono::ceil<std::chrono::seconds>(recast.timestamp - now + recast.recastTime);
    const auto recastSeconds = static_cast<uint32>(std::max<int64>(timer::count_seconds(remaining), 0));

    if (recast.id == Recast::Mount) // borrowing this id for mount recast
    {
        plan.mountRecast   = recastSeconds;
        plan.mountRecastId = static_cast<uint32_t>(recast.id);
    }
    else if (recast.id != Recast::Special)
    {
        auto& entryTimer   = plan.timers[normalCount];
        entryTimer.Timer   = static_cast<uint16>(recastSeconds);
        entryTimer.TimerId = static_cast<uint8>(recast.id);

        if (recast.maxCharges != 0 && recast.hasBaseCharge)
        {
            const uint16 actualChargeTime = timer::count_seconds(recast.chargeTime);
            const uint16 baseChargeTime   = timer::count_seconds(recast.baseChargeTime);

            if (baseChargeTime > actualChargeTime)
            {
                entryTimer.Calc1 = 0; // Not used in Ready, QD, Stratagems... Is this never used?
                entryTimer.Calc2 = static_cast<uint16>(65536 - (baseChargeTime - actualChargeTime) * recast.maxCharges);
            }
        }
        normalCount++;
    }
    else // 2hr edge case // TODO: retail uses Calc2 on 2hr for some reason...
    {
        plan.timers[0].Timer   = static_cast<uint16>(recastSeconds);
        plan.timers[0].TimerId = 0;
    }

    return normalCount > 30;
}

GP_SERV_COMMAND_ABIL_RECAST::GP_SERV_COMMAND_ABIL_RECAST(CCharEntity* PChar)
{
    auto& packet = this->data();

    abilrecasthelpers::Plan plan{};
    uint8                   count      = 1;
    const RecastList_t*     RecastList = PChar->PRecastContainer->GetRecastList(RECAST_ABILITY);
    for (auto&& recast : *RecastList)
    {
        abilrecasthelpers::RecastFact fact{
            .id         = recast.ID,
            .timestamp  = recast.TimeStamp,
            .recastTime = recast.RecastTime,
            .chargeTime = recast.chargeTime,
            .maxCharges = recast.maxCharges,
        };
        if (recast.ID != Recast::Mount && recast.ID != Recast::Special && recast.maxCharges != 0)
        {
            if (const auto* charge = ability::GetCharge(PChar, static_cast<uint16>(recast.ID)))
            {
                fact.hasBaseCharge  = true;
                fact.baseChargeTime = charge->chargeTime;
            }
        }

        // Retail currently only allows 31 distinct recasts to be sent in the packet
        // Reject 32 abilities and higher (zero-indexed)
        // This may change with Master Levels, as there is some padding that appears to be not used for each recast that could be removed to add more abilities.
        if (abilrecasthelpers::Apply(plan, count, fact, timer::now()))
        {
            ShowWarning("GP_SERV_COMMAND_ABIL_RECAST constructor attempting to send recast packet to player '%s' with > 31 abilities. This is unsupported.", PChar->getName());
            break;
        }
    }

    std::memcpy(packet.Timers, plan.timers, sizeof(packet.Timers));
    packet.MountRecast   = plan.mountRecast;
    packet.MountRecastId = plan.mountRecastId;
}
