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

#include "0x0ea_sit.h"

#include "entities/char_entity.h"
#include "entities/pet_entity.h"
#include "status_effect_container.h"

auto sit::TransitionFor(const uint32_t mode, const uint8_t currentAnimation, const PetKind petKind) -> Transition
{
    auto transition = Transition{ .characterAnimation = currentAnimation };

    switch (static_cast<GP_CLI_COMMAND_SIT_MODE>(mode))
    {
        case GP_CLI_COMMAND_SIT_MODE::Toggle:
            transition.characterAnimation = currentAnimation == ANIMATION_SIT ? ANIMATION_NONE : ANIMATION_SIT;
            break;
        case GP_CLI_COMMAND_SIT_MODE::On:
            transition.characterAnimation = ANIMATION_SIT;
            break;
        case GP_CLI_COMMAND_SIT_MODE::Off:
            transition.characterAnimation = ANIMATION_NONE;
            break;
    }

    transition.updatePet = petKind == PetKind::WYVERN || petKind == PetKind::AUTOMATON;
    return transition;
}

// Go host pure half: packetsystem.ValidateSit / ProcessSit /
// NewSitHandler (6503); plan mappacket.ClientSitTransitionFor.
auto GP_CLI_COMMAND_SIT::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::AbnormalStatus, BlockedState::Crafting, BlockedState::PreventAction })
        .oneOf<GP_CLI_COMMAND_SIT_MODE>(this->Mode);
}

void GP_CLI_COMMAND_SIT::process(MapSession* PSession, CCharEntity* PChar) const
{
    auto petKind = sit::PetKind::None;
    auto* PPet   = dynamic_cast<CPetEntity*>(PChar->PPet);
    if (PPet)
    {
        switch (PPet->getPetType())
        {
            case PET_TYPE::WYVERN:
                petKind = sit::PetKind::WYVERN;
                break;
            case PET_TYPE::AUTOMATON:
                petKind = sit::PetKind::AUTOMATON;
                break;
            default:
                petKind = sit::PetKind::Other;
                break;
        }
    }

    const auto transition = sit::TransitionFor(this->Mode, PChar->animation, petKind);

    // Retail accurate: Can inject /sit while healing/logging out, but it cancels the effect.
    if (transition.removeHealingSilently)
    {
        PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Healing);
    }

    PChar->animation = transition.characterAnimation;
    if (transition.updateCharacterHP)
    {
        PChar->updatemask |= UPDATE_HP;
    }

    if (PPet && transition.updatePet)
    {
        PPet->animation = PChar->animation;
        PPet->updatemask |= UPDATE_HP;
    }
}
