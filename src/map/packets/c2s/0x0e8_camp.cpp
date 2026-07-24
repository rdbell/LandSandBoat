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

#include "0x0e8_camp.h"

#include "0x0e7_reqlogout.h"
#include "ai/ai_container.h"
#include "entities/char_entity.h"
#include "status_effect_container.h"

auto camp::HealingTransitionFor(uint32_t mode, bool isHealing, bool hasAutomatonPet, uint8_t healingTickDelay) -> HealingTransition
{
    const auto enableHealing = [hasAutomatonPet, healingTickDelay]
    {
        return HealingTransition{
            .action                = HealingAction::Add,
            .clearStateStack       = true,
            .disengageAutomatonPet = hasAutomatonPet,
            .tick                  = std::chrono::seconds{ healingTickDelay },
        };
    };

    switch (static_cast<GP_CLI_COMMAND_CAMP_MODE>(mode))
    {
        case GP_CLI_COMMAND_CAMP_MODE::Toggle:
            return isHealing ? HealingTransition{ .action = HealingAction::Remove } : enableHealing();
        case GP_CLI_COMMAND_CAMP_MODE::On:
            return enableHealing();
        case GP_CLI_COMMAND_CAMP_MODE::Off:
            return { .action = HealingAction::Remove };
    }

    return {};
}

// Go host pure half: packetsystem.ValidateCamp / ProcessCamp /
// NewCampHandler (6502); plan mappacket.ClientCampHealingTransitionFor.
auto GP_CLI_COMMAND_CAMP::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::AbnormalStatus, BlockedState::Crafting, BlockedState::PreventAction, BlockedState::Engaged })
        .oneOf<GP_CLI_COMMAND_REQLOGOUT_MODE>(this->Mode)
        .mustNotEqual(
            PChar->animation == ANIMATION_HEALING &&
                this->Mode == static_cast<uint32_t>(GP_CLI_COMMAND_CAMP_MODE::On),
            true,
            "Requested healing when already healing")
        .mustNotEqual(
            PChar->animation != ANIMATION_HEALING &&
                this->Mode == static_cast<uint32_t>(GP_CLI_COMMAND_CAMP_MODE::Off),
            true,
            "Requested stop healing when not healing");
}

void GP_CLI_COMMAND_CAMP::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto hasAutomatonPet = PChar->PPet &&
                                 PChar->PPet->objtype == TYPE_PET &&
                                 static_cast<CPetEntity*>(PChar->PPet)->getPetType() == PET_TYPE::AUTOMATON;
    const auto transition = camp::HealingTransitionFor(
        this->Mode,
        PChar->animation == ANIMATION_HEALING,
        hasAutomatonPet,
        settings::get<uint8>("map.HEALING_TICK_DELAY"));

    if (transition.clearStateStack)
    {
        PChar->PAI->ClearStateStack();
    }
    if (transition.disengageAutomatonPet)
    {
        PChar->PPet->PAI->Disengage();
    }

    // Note: The status effect lua takes care of changing the animation.
    switch (transition.action)
    {
        case camp::HealingAction::Add:
            PChar->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Healing, 0, 0, transition.tick, 0s);
            break;
        case camp::HealingAction::Remove:
            PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Healing);
            break;
        case camp::HealingAction::None:
            break;
    }
}
