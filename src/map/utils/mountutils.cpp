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

#include "mountutils.h"

#include "utils/mount_capacity.h"

#include "entities/base_entity.h"
#include "entities/char_entity.h"
#include "status_effect.h"
#include "status_effect_container.h"

namespace mountutils
{

auto resolveState(const CStatusEffect* effect) -> MountStateResolution
{
    // Pure policy dual-wire: mountutilshelpers::PlanResolveMountState (slice 2839).
    if (!effect)
    {
        return mountutilshelpers::PlanResolveMountState(true, 0, 0, MOUNT_CHOCOBO, MOUNT_NOBLE_CHOCOBO);
    }

    const auto mount = effect->GetPower();
    return mountutilshelpers::PlanResolveMountState(false, mount, effect->GetSubPower(), MOUNT_CHOCOBO, MOUNT_NOBLE_CHOCOBO);
}

// ChocoboIndex is a field (0-7) used in various packets.
// While it has little incidence for mounts, it is extremely important for custom chocobos.
// CustomProperties[0] sets the Personal Chocobo model.
// CustomProperties[1] is used for Noble Chocobo, and is set to 1.
//
// Pure policy dual-wire: mountutilshelpers::PlanMountPacketDefinition (slice 2844).
// Host injects mounted/mount from resolveState and m_FieldChocobo.
auto packetDefinition(const CCharEntity* PChar) -> MountPacketDefinition
{
    const auto state = resolveState(PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Mounted));
    return mountutilshelpers::PlanMountPacketDefinition(state.mounted, state.mount, PChar->m_FieldChocobo);
}

}; // namespace mountutils
