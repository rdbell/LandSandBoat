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

#include "0x0a0_map_group.h"

#include "common/logging.h"
#include "entities/char_entity.h"

auto mapgroupserverhelpers::PlanFor(const Facts& facts) -> GP_SERV_COMMAND_MAP_GROUP::PacketData
{
    return {
        .UniqueID = facts.uniqueId,
        .zone     = facts.zone,
        .x        = facts.x,
        .y        = facts.y,
        .z        = facts.z,
    };
}

GP_SERV_COMMAND_MAP_GROUP::GP_SERV_COMMAND_MAP_GROUP(const CCharEntity* PChar)
{
    if (PChar == nullptr)
    {
        ShowError("GP_SERV_COMMAND_MAP_GROUP::GP_SERV_COMMAND_MAP_GROUP() - PChar was null.");
        return;
    }

    this->data() = mapgroupserverhelpers::PlanFor({
        .uniqueId = PChar->id,
        .zone     = static_cast<int16>(PChar->getZone()),
        .x        = PChar->loc.p.x,
        .y        = PChar->loc.p.y,
        .z        = PChar->loc.p.z,
    });
}
