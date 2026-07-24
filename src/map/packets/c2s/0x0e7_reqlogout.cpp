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

#include "0x0e7_reqlogout.h"

#include "entities/char_entity.h"
#include "status_effect_container.h"
#include "utils/charutils.h"

auto reqlogout::LeaveGameTransitionFor(uint16_t mode, uint16_t kind, bool hasExistingEffect) -> LeaveGameTransition
{
    const auto makeAdd = [](GP_CLI_COMMAND_REQLOGOUT_KIND leaveGameKind)
    {
        return LeaveGameTransition{
            .action   = LeaveGameAction::Add,
            .power    = static_cast<uint16_t>(leaveGameKind),
            .duration = std::chrono::seconds{ 5 },
            .tick     = std::chrono::seconds{ 0 },
        };
    };
    const auto makeUpdate = [](GP_CLI_COMMAND_REQLOGOUT_KIND leaveGameKind)
    {
        return LeaveGameTransition{
            .action = LeaveGameAction::UpdatePower,
            .power  = static_cast<uint16_t>(leaveGameKind),
        };
    };

    const auto reqLogoutMode = static_cast<GP_CLI_COMMAND_REQLOGOUT_MODE>(mode);
    const auto reqLogoutKind = static_cast<GP_CLI_COMMAND_REQLOGOUT_KIND>(kind);

    switch (reqLogoutKind)
    {
        case GP_CLI_COMMAND_REQLOGOUT_KIND::Logout:
            switch (reqLogoutMode)
            {
                case GP_CLI_COMMAND_REQLOGOUT_MODE::Toggle:
                    return hasExistingEffect ? LeaveGameTransition{ .action = LeaveGameAction::Remove } : makeAdd(reqLogoutKind);
                case GP_CLI_COMMAND_REQLOGOUT_MODE::LogoutOn:
                    return hasExistingEffect ? makeUpdate(reqLogoutKind) : makeAdd(reqLogoutKind);
                case GP_CLI_COMMAND_REQLOGOUT_MODE::Off:
                    return hasExistingEffect ? LeaveGameTransition{ .action = LeaveGameAction::Remove } : LeaveGameTransition{};
                case GP_CLI_COMMAND_REQLOGOUT_MODE::ShutdownOn:
                    return {};
            }
            break;
        case GP_CLI_COMMAND_REQLOGOUT_KIND::Shutdown:
            switch (reqLogoutMode)
            {
                case GP_CLI_COMMAND_REQLOGOUT_MODE::Toggle:
                    return hasExistingEffect ? LeaveGameTransition{ .action = LeaveGameAction::Remove } : makeAdd(reqLogoutKind);
                case GP_CLI_COMMAND_REQLOGOUT_MODE::LogoutOn:
                    return {};
                case GP_CLI_COMMAND_REQLOGOUT_MODE::Off:
                    return hasExistingEffect ? LeaveGameTransition{ .action = LeaveGameAction::Remove } : LeaveGameTransition{};
                case GP_CLI_COMMAND_REQLOGOUT_MODE::ShutdownOn:
                    return hasExistingEffect ? makeUpdate(reqLogoutKind) : makeAdd(reqLogoutKind);
            }
            break;
    }

    return {};
}

// Go host pure half: packetsystem.ValidateReqLogout / ProcessReqLogout /
// NewReqLogoutHandler (6501); plan mappacket.ClientReqLogoutLeaveGameTransitionFor.
auto GP_CLI_COMMAND_REQLOGOUT::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::AbnormalStatus, BlockedState::Crafting, BlockedState::PreventAction })
        .oneOf<GP_CLI_COMMAND_REQLOGOUT_MODE>(this->Mode)
        .oneOf<GP_CLI_COMMAND_REQLOGOUT_KIND>(this->Kind);
}

void GP_CLI_COMMAND_REQLOGOUT::process(MapSession* PSession, CCharEntity* PChar) const
{
    auto* existingEffect = PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Leavegame);
    const auto transition = reqlogout::LeaveGameTransitionFor(this->Mode, this->Kind, existingEffect != nullptr);

    switch (transition.action)
    {
        case reqlogout::LeaveGameAction::Add:
            PChar->StatusEffectContainer->AddStatusEffect(xi::StatusEffect::Leavegame, 0, transition.power, transition.duration, transition.tick);
            break;
        case reqlogout::LeaveGameAction::UpdatePower:
            existingEffect->SetPower(transition.power);
            break;
        case reqlogout::LeaveGameAction::Remove:
            PChar->StatusEffectContainer->DelStatusEffectSilent(xi::StatusEffect::Leavegame);
            break;
        case reqlogout::LeaveGameAction::None:
            break;
    }
}
