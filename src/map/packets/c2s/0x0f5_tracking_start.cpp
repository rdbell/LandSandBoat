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

#include "0x0f5_tracking_start.h"

#include "tracking_transitions.h"

#include "entities/char_entity.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateTrackingStart / ProcessTrackingStart / NewTrackingStartHandler (6530); plan mappacket.ClientTrackingStartRuntimePlanFor.
auto GP_CLI_COMMAND_TRACKING_START::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .range("ActIndex", this->ActIndex, 0x1, 0x1000); // 1 to 4096
}

void GP_CLI_COMMAND_TRACKING_START::process(MapSession* PSession, CCharEntity* PChar) const
{
    CBaseEntity* target = PChar->GetEntity(this->ActIndex, TYPE_MOB | TYPE_NPC);
    Maybe<tracking::TargetIdentity> requestedTarget;
    bool                            isWideScannable{};
    bool                            isWithinRange{};

    if (target != nullptr)
    {
        const float dist = distance(PChar->loc.p, target->loc.p);
        requestedTarget = tracking::TargetIdentity{
            .id     = target->id,
            .targid = target->targid
        };
        isWideScannable = target->isWideScannable();
        isWithinRange   = dist <= charutils::getWideScanRange(PChar);
    }

    Maybe<tracking::TargetIdentity> currentTarget;
    if (PChar->WideScanTarget)
    {
        currentTarget = tracking::TargetIdentity{
            .id     = PChar->WideScanTarget->id,
            .targid = PChar->WideScanTarget->targid
        };
    }

    const auto nextTarget = tracking::StartTargetFor(currentTarget, requestedTarget, isWideScannable, isWithinRange);
    if (nextTarget)
    {
        PChar->WideScanTarget = EntityID_t{ .id = nextTarget->id, .targid = nextTarget->targid };
    }
    else
    {
        PChar->WideScanTarget = std::nullopt;
    }
}
