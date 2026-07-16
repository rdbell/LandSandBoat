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

#include "0x065_wpos2.h"
#include "wpos_runtime.h"

#include "entities/base_entity.h"
#include "entities/char_entity.h"

GP_SERV_COMMAND_WPOS2::GP_SERV_COMMAND_WPOS2(CBaseEntity* PEntity, const position_t position, POSMODE mode)
{
    auto& packet = this->data();

    // Applying the transition first prevents conflicts when the client receives the packet.
    auto*      PChar = dynamic_cast<CCharEntity*>(PEntity);
    const auto plan  = wposhelpers::PlanFor({ .current = PEntity->loc.p, .character = PChar != nullptr }, position, mode);
    PEntity->loc.p   = plan.current;
    if (PChar && (mode == POSMODE::RESET || mode == POSMODE::LOCK || mode == POSMODE::UNLOCK))
    {
        PChar->setLocked(plan.locked);
    }

    packet.x        = PEntity->loc.p.x;
    packet.y        = PEntity->loc.p.y;
    packet.z        = PEntity->loc.p.z;
    packet.dir      = PEntity->loc.p.rotation;
    packet.UniqueNo = PEntity->id;
    packet.ActIndex = PEntity->targid;
    packet.Mode     = static_cast<uint8_t>(mode);
}
