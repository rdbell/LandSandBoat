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

#include "0x113_sitchair.h"

#include "entities/char_entity.h"
#include "status_effect_container.h"
#include "utils/charutils.h"
#include "validation.h"

// Go host pure half: packetsystem.ValidateSitChair / ProcessSitChair /
// NewSitChairHandler (6498); plan mappacket.ClientSitChairRuntimePlanFor.
auto GP_CLI_COMMAND_SITCHAIR::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::AbnormalStatus, BlockedState::Crafting, BlockedState::PreventAction })
        .oneOf<GP_CLI_COMMAND_SITCHAIR_MODE>(this->Mode)
        .range("ChairId", this->ChairId, 0, 20); // 10 chairs + 10 reserved slots for future use
}

void GP_CLI_COMMAND_SITCHAIR::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto chairAnimation = static_cast<uint8_t>(this->ChairId + ANIMATION_SITCHAIR_0);
    const auto hasRequiredChairKeyItem = !sitchairhelpers::RequiresChairKeyItem(chairAnimation) ||
                                         charutils::hasKeyItem(PChar, static_cast<KeyItem>(sitchairhelpers::ChairKeyItemID(chairAnimation)));
    const auto transition = sitchairhelpers::SelectTransition(this->Mode, chairAnimation, PChar->animation, hasRequiredChairKeyItem);

    // Retail accurate: Can inject /sitchair while healing/logging out, but it cancels the effect.
    PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Healing);
    PChar->animation = transition.animation;
    PChar->updatemask |= UPDATE_HP;
}
