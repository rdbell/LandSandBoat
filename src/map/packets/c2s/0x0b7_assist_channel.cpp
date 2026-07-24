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

#include "0x0b7_assist_channel.h"

#include "aman.h"
#include "common/ipc_structs.h"
#include "entities/char_entity.h"
#include "ipc_client.h"
#include "utils/charutils.h"

auto assistchannelhelpers::SelectAction(const GP_CLI_COMMAND_ASSIST_CHANNEL_KIND kind, const bool targetFound, const bool authorized, const bool cooldownReady) -> AssistChannelAction
{
    // Name resolution precedes the action switch in process, including for an
    // unexpected kind which validation normally rejects.
    if (!targetFound)
    {
        return AssistChannelAction::Error;
    }

    switch (kind)
    {
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp:
            if (!authorized)
            {
                return AssistChannelAction::Error;
            }
            return cooldownReady ? AssistChannelAction::Forward : AssistChannelAction::ThumbsUpCooldown;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning:
            if (!authorized)
            {
                return AssistChannelAction::Error;
            }
            return cooldownReady ? AssistChannelAction::Forward : AssistChannelAction::WarningCooldown;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList:
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList:
            return authorized ? AssistChannelAction::Forward : AssistChannelAction::Error;
        default:
            return AssistChannelAction::None;
    }
}

// Go host pure half: packetsystem.ValidateAssistChannel / ProcessAssistChannel /
// NewAssistChannelHandler (6500); plan mappacket.ClientAssistChannelActionFor.
auto GP_CLI_COMMAND_ASSIST_CHANNEL::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf<GP_CLI_COMMAND_ASSIST_CHANNEL_KIND>(this->Kind)
        .mustEqual(settings::get<bool>("main.ASSIST_CHANNEL_ENABLED"), true, "Assist Channel is not enabled");
}

void GP_CLI_COMMAND_ASSIST_CHANNEL::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto safeName = db::escapeString(asStringFromUntrustedSource(this->sName, sizeof(this->sName)));

    const auto victimId = charutils::getCharIdFromName(safeName);
    if (!victimId)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(MsgStd::AnErrorHasOccured);
        return;
    }

    const auto kind = static_cast<GP_CLI_COMMAND_ASSIST_CHANNEL_KIND>(this->Kind);
    auto       action = AssistChannelAction::None;
    switch (kind)
    {
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::GiveThumbsUp:
        {
            const auto muted = PChar->aman().isMuted();
            action = assistchannelhelpers::SelectAction(kind, true, !muted, !muted && PChar->aman().canThumbsUp());
        }
        break;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::IssueWarning:
        {
            const auto authorized = PChar->aman().isMentor() && !PChar->aman().isMuted();
            action = assistchannelhelpers::SelectAction(kind, true, authorized, authorized && PChar->aman().canIssueWarning());
        }
        break;
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::AddToMuteList:
        case GP_CLI_COMMAND_ASSIST_CHANNEL_KIND::RemoveFromMuteList:
        {
            const auto authorized = PChar->aman().isMentor() && PChar->aman().getMasteryRank() >= 6 && !PChar->aman().isMuted();
            action = assistchannelhelpers::SelectAction(kind, true, authorized, true);
        }
        break;
    }

    switch (action)
    {
        case AssistChannelAction::Error:
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(MsgStd::AnErrorHasOccured);
            return;
        case AssistChannelAction::ThumbsUpCooldown:
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(MsgStd::ThumbsUpCooldown);
            return;
        case AssistChannelAction::WarningCooldown:
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(MsgStd::WarningCooldown);
            return;
        case AssistChannelAction::Forward:
            // TODO: If char is offline, this is an automatic fail.
            message::send(ipc::AssistChannelEvent{
                .senderId   = PChar->id,
                .receiverId = victimId,
                .action     = static_cast<uint8>(kind),
            });
            return;
        case AssistChannelAction::None:
            return;
    }
}
