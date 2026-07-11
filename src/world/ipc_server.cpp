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

#include "ipc_server.h"

#include "account_login.h"
#include "alliance_dissolve.h"
#include "alliance_members_reroute.h"
#include "alliance_reload.h"
#include "char_id_reroute.h"
#include "char_name_reroute.h"
#include "char_var_update.h"
#include "char_zone.h"
#include "chat_message_alliance.h"
#include "chat_message_assist.h"
#include "chat_message_custom.h"
#include "chat_message_linkshell.h"
#include "chat_message_party.h"
#include "chat_message_server_message.h"
#include "chat_message_tell.h"
#include "chat_message_unity.h"
#include "chat_message_yell.h"
#include "kill_session.h"
#include "linkshell_members_reroute.h"
#include "linkshell_rank_change.h"
#include "linkshell_remove.h"
#include "linkshell_set_message.h"
#include "lua_function.h"
#include "message_standard.h"
#include "message_system.h"
#include "party_invite.h"
#include "party_invite_response.h"
#include "party_disband.h"
#include "party_members_reroute.h"
#include "party_reload.h"
#include "player_kick.h"
#include "zone_id_reroute.h"

#include "besieged_system.h"
#include "campaign_system.h"
#include "character_cache.h"
#include "colonization_system.h"
#include "conquest_system.h"
#include "gmcall_request.h"
#include "gmcall_response.h"
#include "gmcall_response_persistence.h"
#include "ipc_lookup.h"

#include <concurrentqueue.h>
#include <memory>

#include "common/database.h"
#include "common/logging.h"

namespace
{

auto getZMQEndpointString() -> std::string
{
    return fmt::format(
        "{}://{}:{}",
        settings::get<std::string>("network.ZMQ_TRANSPORT"),
        settings::get<std::string>("network.ZMQ_IP"),
        settings::get<uint16>("network.ZMQ_PORT"));
}

} // namespace

IPCServer::IPCServer(WorldEngine& worldServer, ZMQService& zmqService)
: worldServer_(worldServer)
, channel_(zmqService.registerRouter(getZMQEndpointString()))
{
    TracyZoneScoped;
}

//
// IPP Lookup
//

auto IPCServer::getIPPForCharId(uint32 charId) -> Maybe<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info
    // if (const auto cachedIPP = characterCache_.getCharacterIPP(charId))
    // {
    //     return *cachedIPP;
    // }

    return world::ipc::LookupCharacterEndpoint(charId);
}

auto IPCServer::getIPPForCharName(const std::string& charName) -> Maybe<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info

    return world::ipc::LookupCharacterNameEndpoint(charName);
}

auto IPCServer::getIPPForZoneId(uint16 zoneId) -> Maybe<IPP>
{
    TracyZoneScoped;

    // TODO: Using the cache we can know if a whole process has no active players on it,
    //     : so we could omit forwarding messages to it

    if (const auto it = zoneSettings_.zoneSettingsMap_.find(zoneId); it != zoneSettings_.zoneSettingsMap_.end())
    {
        return it->second.ipp;
    }

    return std::nullopt;
}

auto IPCServer::getIPPsForParty(uint32 partyId) -> std::vector<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info

    return world::ipc::LookupPartyEndpoints(partyId);
}

auto IPCServer::getIPPsForAlliance(uint32 allianceId) -> std::vector<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info

    return world::ipc::LookupAllianceEndpoints(allianceId);
}

auto IPCServer::getIPPsForLinkshell(uint32 linkshellId) -> std::vector<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info

    return world::ipc::LookupLinkshellEndpoints(linkshellId);
}

auto IPCServer::getIPPsForUnity(uint32 unityId) -> std::vector<IPP>
{
    TracyZoneScoped;

    // TODO: We know when chars move, we could be caching this info

    return world::ipc::LookupUnityEndpoints(unityId);
}

auto IPCServer::getIPPsForYellZones() -> std::vector<IPP>
{
    TracyZoneScoped;

    return zoneSettings_.yellMapEndpoints_;
}

auto IPCServer::getIPPsForAssistZones() -> std::vector<IPP>
{
    TracyZoneScoped;

    return zoneSettings_.assistMapEndpoints_;
}

auto IPCServer::getIPPsForAllZones() -> std::vector<IPP>
{
    TracyZoneScoped;

    // TODO: Using the cache we can know if a whole process has no active players on it,
    //     : so we could omit forwarding messages to it

    return zoneSettings_.mapEndpoints_;
}

//
// Message routing
//

void IPCServer::rerouteMessageToCharId(uint32 charId, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToCharId(
        charId,
        message,
        [this](const uint32 targetId)
        {
            return getIPPForCharId(targetId);
        },
        [this, charId](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to char<{}> on {}", charId, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToCharName(const std::string& charName, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToCharName(
        charName,
        message,
        [this](const std::string& targetName)
        {
            return getIPPForCharName(targetName);
        },
        [this, &charName](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to char<{}> on {}", charName, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToZoneId(uint16 zoneId, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToZoneId(
        zoneId,
        message,
        [this](uint16 targetZoneId)
        {
            return getIPPForZoneId(targetZoneId);
        },
        [this, zoneId](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to zone<{}> on {}", zoneId, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToPartyMembers(uint32 partyId, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToPartyMembers(
        partyId,
        message,
        [this](const uint32 targetId)
        {
            return getIPPsForParty(targetId);
        },
        [this, partyId](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to party<{}> on {}", partyId, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToAllianceMembers(uint32 allianceId, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToAllianceMembers(
        allianceId,
        message,
        [this](const uint32 targetId)
        {
            return getIPPsForAlliance(targetId);
        },
        [this, allianceId](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to alliance<{}> on {}", allianceId, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToLinkshellMembers(uint32 linkshellId, const auto& message)
{
    TracyZoneScoped;

    worldipc::RerouteMessageToLinkshellMembers(
        linkshellId,
        message,
        [this](uint32 targetLinkshellId)
        {
            return getIPPsForLinkshell(targetLinkshellId);
        },
        [this, linkshellId](const IPP& endpoint, const auto& delivered)
        {
            DebugIPCFmt("Message: -> rerouting to linkshell<{}> on {}", linkshellId, endpoint.toString());
            this->sendMessage(endpoint, delivered);
        });
}

void IPCServer::rerouteMessageToUnityMembers(uint32 unityId, const auto& message)
{
    TracyZoneScoped;

    for (const auto& ipp : getIPPsForUnity(unityId))
    {
        DebugIPCFmt("Message: -> rerouting to unity<{}> on {}", unityId, ipp.toString());
        sendMessage(ipp, message);
    }
}

void IPCServer::rerouteMessageToYellZones(const auto& message)
{
    TracyZoneScoped;

    for (const auto& ipp : getIPPsForYellZones())
    {
        DebugIPCFmt("Message: -> rerouting to yell zone on {}", ipp.toString());
        sendMessage(ipp, message);
    }
}

void IPCServer::rerouteMessageToAssistZones(const auto& message)
{
    TracyZoneScoped;

    for (const auto& ipp : getIPPsForAssistZones())
    {
        DebugIPCFmt("Message: -> rerouting to assist zone on {}", ipp.toString());
        sendMessage(ipp, message);
    }
}

void IPCServer::rerouteMessageToAllZones(const auto& message)
{
    TracyZoneScoped;

    for (const auto& ipp : getIPPsForAllZones())
    {
        DebugIPCFmt("Message: -> rerouting to all zones on {}", ipp.toString());
        sendMessage(ipp, message);
    }
}

void IPCServer::handleIncomingMessages()
{
    TracyZoneScoped;

    // TODO: Should we stop more messages appearing on the queue while we're processing?
    IPPMessage message;
    while (channel_.tryReceive(message))
    {
        const auto firstByte = message.payload[0];
        const auto msgType   = ipc::toString(static_cast<ipc::MessageType>(firstByte));

        DebugIPCFmt("Incoming {} message from {}", msgType, message.ipp.toString());

        handleMessage(message.ipp, { message.payload.data(), message.payload.size() });
    }
}

void IPCServer::handleMessage_EmptyStruct(const IPP& ipp, const ipc::EmptyStruct& message)
{
    TracyZoneScoped;

    ShowWarningFmt("Received EmptyStruct message from {} - this is probably a bug", ipp.toString());
}

void IPCServer::handleMessage_AccountLogin(const IPP& ipp, const ipc::AccountLogin& message)
{
    TracyZoneScoped;

    DebugIPCFmt("Received AccountLogin message from {} for account {}", ipp.toString(), message.accountId);

    worldipc::HandleAccountLogin(
        message,
        getIPPsForAllZones(),
        [this, &ipp](const IPP& endpoint, const ipc::AccountLogin& accountLogin)
        {
            DebugIPCFmt("Message: -> rerouting to all zones on {}", ipp.toString());
            sendMessage(endpoint, accountLogin);
        });
}

void IPCServer::handleMessage_CharZone(const IPP& ipp, const ipc::CharZone& message)
{
    TracyZoneScoped;

    worldipc::HandleCharZone(
        message,
        [this](const uint32 charId)
        {
            characterCache_.removeCharacter(charId);
        },
        [this](const uint16 zoneId)
        {
            return getIPPForZoneId(zoneId);
        },
        [this](const uint32 charId, const IPP& endpoint)
        {
            characterCache_.updateCharacter(charId, endpoint);
        },
        [this](const uint16 zoneId, const ipc::CharZone& charZone)
        {
            rerouteMessageToZoneId(zoneId, charZone);
        });
}

void IPCServer::handleMessage_CharVarUpdate(const IPP& ipp, const ipc::CharVarUpdate& message)
{
    TracyZoneScoped;

    worldipc::HandleCharVarUpdate(
        message,
        [this](const uint32 charId, const ipc::CharVarUpdate& update)
        {
            rerouteMessageToCharId(charId, update);
        });
}

void IPCServer::handleMessage_ChatMessageTell(const IPP& ipp, const ipc::ChatMessageTell& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageTell(
        ipp,
        message,
        [this](const std::string& recipientName)
        {
            return getIPPForCharName(recipientName);
        },
        [this](const IPP& endpoint, const ipc::ChatMessageTell& tell)
        {
            this->sendMessage(endpoint, tell);
        },
        [this](const IPP& endpoint, const ipc::MessageStandard& offline)
        {
            this->sendMessage(endpoint, offline);
        });
}

void IPCServer::handleMessage_ChatMessageParty(const IPP& ipp, const ipc::ChatMessageParty& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageParty(
        message,
        [this](const uint32 partyId)
        {
            return getIPPsForParty(partyId);
        },
        [this](const IPP& endpoint, const ipc::ChatMessageParty& partyMessage)
        {
            DebugIPCFmt("Message: -> rerouting to party<{}> on {}", partyMessage.partyId, endpoint.toString());
            sendMessage(endpoint, partyMessage);
        });
}

void IPCServer::handleMessage_ChatMessageAlliance(const IPP& ipp, const ipc::ChatMessageAlliance& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageAlliance(
        message,
        [this](const uint32 allianceId)
        {
            return getIPPsForAlliance(allianceId);
        },
        [this](const IPP& endpoint, const ipc::ChatMessageAlliance& allianceMessage)
        {
            DebugIPCFmt("Message: -> rerouting to alliance<{}> on {}", allianceMessage.allianceId, endpoint.toString());
            sendMessage(endpoint, allianceMessage);
        });
}

void IPCServer::handleMessage_ChatMessageLinkshell(const IPP& ipp, const ipc::ChatMessageLinkshell& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageLinkshell(
        message,
        [this](const uint32 linkshellId)
        {
            return getIPPsForLinkshell(linkshellId);
        },
        [this](const IPP& endpoint, const ipc::ChatMessageLinkshell& linkshellMessage)
        {
            DebugIPCFmt("Message: -> rerouting to linkshell<{}> on {}", linkshellMessage.linkshellId, endpoint.toString());
            sendMessage(endpoint, linkshellMessage);
        });
}

void IPCServer::handleMessage_ChatMessageUnity(const IPP& ipp, const ipc::ChatMessageUnity& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageUnity(
        message,
        [this](const uint32 unityId)
        {
            return getIPPsForUnity(unityId);
        },
        [this](const IPP& endpoint, const ipc::ChatMessageUnity& unityMessage)
        {
            DebugIPCFmt("Message: -> rerouting to unity<{}> on {}", unityMessage.unityLeaderId, endpoint.toString());
            sendMessage(endpoint, unityMessage);
        });
}

void IPCServer::handleMessage_ChatMessageYell(const IPP& ipp, const ipc::ChatMessageYell& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageYell(
        message,
        [this]
        {
            return getIPPsForYellZones();
        },
        [this](const IPP& endpoint, const ipc::ChatMessageYell& yellMessage)
        {
            DebugIPCFmt("Message: -> rerouting to yell zone on {}", endpoint.toString());
            sendMessage(endpoint, yellMessage);
        });
}

void IPCServer::handleMessage_ChatMessageAssist(const IPP& ipp, const ipc::ChatMessageAssist& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageAssist(
        message,
        [this]
        {
            return getIPPsForAssistZones();
        },
        [this](const IPP& endpoint, const ipc::ChatMessageAssist& assistMessage)
        {
            DebugIPCFmt("Message: -> rerouting to assist zone on {}", endpoint.toString());
            sendMessage(endpoint, assistMessage);
        });
}

void IPCServer::handleMessage_ChatMessageServerMessage(const IPP& ipp, const ipc::ChatMessageServerMessage& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageServerMessage(
        message,
        [this]
        {
            return getIPPsForAllZones();
        },
        [this](const IPP& endpoint, const ipc::ChatMessageServerMessage& serverMessage)
        {
            DebugIPCFmt("Message: -> rerouting to all zones on {}", endpoint.toString());
            sendMessage(endpoint, serverMessage);
        });
}

void IPCServer::handleMessage_ChatMessageCustom(const IPP& ipp, const ipc::ChatMessageCustom& message)
{
    TracyZoneScoped;

    worldipc::HandleChatMessageCustom(
        message,
        [this](const uint32 recipientId, const ipc::ChatMessageCustom& customMessage)
        {
            rerouteMessageToCharId(recipientId, customMessage);
        });
}

void IPCServer::handleMessage_PartyInvite(const IPP& ipp, const ipc::PartyInvite& message)
{
    TracyZoneScoped;

    worldipc::HandlePartyInvite(
        message,
        [this](const uint32 inviteeId, const ipc::PartyInvite& invite)
        {
            rerouteMessageToCharId(inviteeId, invite);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_PartyInviteResponse(const IPP& ipp, const ipc::PartyInviteResponse& message)
{
    TracyZoneScoped;

    worldipc::HandlePartyInviteResponse(
        message,
        [this](const uint32 inviterId, const ipc::PartyInviteResponse& response)
        {
            rerouteMessageToCharId(inviterId, response);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_PartyReload(const IPP& ipp, const ipc::PartyReload& message)
{
    TracyZoneScoped;

    worldipc::HandlePartyReload(
        message,
        [this](const uint32 partyId, const ipc::PartyReload& reload)
        {
            rerouteMessageToPartyMembers(partyId, reload);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_PartyDisband(const IPP& ipp, const ipc::PartyDisband& message)
{
    TracyZoneScoped;

    worldipc::HandlePartyDisband(
        message,
        [this](const uint32 partyId, const ipc::PartyDisband& disband)
        {
            rerouteMessageToPartyMembers(partyId, disband);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_AllianceReload(const IPP& ipp, const ipc::AllianceReload& message)
{
    TracyZoneScoped;

    worldipc::HandleAllianceReload(
        message,
        [this](const uint32 allianceId, const ipc::AllianceReload& reload)
        {
            rerouteMessageToAllianceMembers(allianceId, reload);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_AllianceDissolve(const IPP& ipp, const ipc::AllianceDissolve& message)
{
    TracyZoneScoped;

    worldipc::HandleAllianceDissolve(
        message,
        [this](const uint32 allianceId, const ipc::AllianceDissolve& dissolve)
        {
            rerouteMessageToAllianceMembers(allianceId, dissolve);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_PlayerKick(const IPP& ipp, const ipc::PlayerKick& message)
{
    TracyZoneScoped;

    worldipc::HandlePlayerKick(
        message,
        [this](const uint32 victimId, const ipc::PlayerKick& kick)
        {
            rerouteMessageToCharId(victimId, kick);
        });

    // TODO:
    // worldServer_.partySystem_->handleMessage(message);
}

void IPCServer::handleMessage_MessageStandard(const IPP& ipp, const ipc::MessageStandard& message)
{
    TracyZoneScoped;

    worldipc::HandleMessageStandard(
        message,
        [this](const uint32 recipientId, const ipc::MessageStandard& standardMessage)
        {
            rerouteMessageToCharId(recipientId, standardMessage);
        });
}

void IPCServer::handleMessage_MessageSystem(const IPP& ipp, const ipc::MessageSystem& message)
{
    TracyZoneScoped;

    worldipc::HandleMessageSystem(
        message,
        [this](const uint32 recipientId, const ipc::MessageSystem& systemMessage)
        {
            rerouteMessageToCharId(recipientId, systemMessage);
        });
}

void IPCServer::handleMessage_LinkshellRankChange(const IPP& ipp, const ipc::LinkshellRankChange& message)
{
    TracyZoneScoped;

    worldipc::HandleLinkshellRankChange(
        message,
        [this](const std::string& memberName, const ipc::LinkshellRankChange& change)
        {
            rerouteMessageToCharName(memberName, change);
        });
}

void IPCServer::handleMessage_LinkshellRemove(const IPP& ipp, const ipc::LinkshellRemove& message)
{
    TracyZoneScoped;

    worldipc::HandleLinkshellRemove(
        message,
        [this](const std::string& victimName, const ipc::LinkshellRemove& removal)
        {
            rerouteMessageToCharName(victimName, removal);
        });
}

void IPCServer::handleMessage_LinkshellSetMessage(const IPP& ipp, const ipc::LinkshellSetMessage& message)
{
    TracyZoneScoped;

    worldipc::HandleLinkshellSetMessage(
        message,
        [this](uint32 linkshellId, const ipc::LinkshellSetMessage& update)
        {
            rerouteMessageToLinkshellMembers(linkshellId, update);
        });
}

void IPCServer::handleMessage_LuaFunction(const IPP& ipp, const ipc::LuaFunction& message)
{
    TracyZoneScoped;

    worldipc::HandleLuaFunction(
        message,
        [this](uint16 executorZoneId, const ipc::LuaFunction& function)
        {
            rerouteMessageToZoneId(executorZoneId, function);
        });
}

void IPCServer::handleMessage_KillSession(const IPP& ipp, const ipc::KillSession& message)
{
    TracyZoneScoped;

    worldipc::HandleKillSession(
        message,
        [](uint32 victimId)
        {
            return world::ipc::LookupKillSessionZones(victimId);
        },
        [this](uint16 previousZoneId)
        {
            return zoneSettings_.zoneSettingsMap_.at(previousZoneId).ipp;
        },
        [this]() -> const std::vector<IPP>&
        {
            return zoneSettings_.mapEndpoints_;
        },
        [this](const IPP& endpoint, const ipc::KillSession& kill)
        {
            DebugIPCFmt("Message: -> rerouting to {}", endpoint.toString());
            sendMessage(endpoint, kill);
        });
}

void IPCServer::handleMessage_ConquestEvent(const IPP& ipp, const ipc::ConquestEvent& message)
{
    TracyZoneScoped;

    worldServer_.conquestSystem_->handleMessage(message.type, { ipp, message.payload });
}

void IPCServer::handleMessage_BesiegedEvent(const IPP& ipp, const ipc::BesiegedEvent& message)
{
    TracyZoneScoped;

    worldServer_.besiegedSystem_->handleMessage(message.type, { ipp, message.payload });
}

void IPCServer::handleMessage_CampaignEvent(const IPP& ipp, const ipc::CampaignEvent& message)
{
    TracyZoneScoped;

    worldServer_.campaignSystem_->handleMessage(message.type, { ipp, message.payload });
}

void IPCServer::handleMessage_ColonizationEvent(const IPP& ipp, const ipc::ColonizationEvent& message)
{
    TracyZoneScoped;

    worldServer_.colonizationSystem_->handleMessage(message.type, { ipp, message.payload });
}

void IPCServer::handleMessage_EntityInformationRequest(const IPP& ipp, const ipc::EntityInformationRequest& message)
{
    TracyZoneScoped;

    // enum ENTITYTYPE : uint8
    // {
    //     TYPE_NONE   = 0x00,
    //     TYPE_PC     = 0x01,
    //     TYPE_NPC    = 0x02,
    //     TYPE_MOB    = 0x04,

    if (message.entityType == 0x01)
    {
        rerouteMessageToCharId(message.targetId, message);
    }
    else
    {
        const auto zoneId = (message.targetId >> 12) & 0x0FFF;
        rerouteMessageToZoneId(zoneId, message);
    }
}

void IPCServer::handleMessage_EntityInformationResponse(const IPP& ipp, const ipc::EntityInformationResponse& message)
{
    TracyZoneScoped;

    rerouteMessageToCharId(message.requesterId, message);
}

void IPCServer::handleMessage_SendPlayerToLocation(const IPP& ipp, const ipc::SendPlayerToLocation& message)
{
    TracyZoneScoped;

    rerouteMessageToCharId(message.targetId, message);
}

void IPCServer::handleMessage_AssistChannelEvent(const IPP& ipp, const ipc::AssistChannelEvent& message)
{
    TracyZoneScoped;

    rerouteMessageToCharId(message.receiverId, message);
}

void IPCServer::handleMessage_GMCallRequest(const IPP& ipp, const ipc::GMCallRequest& message)
{
    TracyZoneScoped;

    ShowInfoFmt("{}", world::gmcall::FormatRequestLog(message));

    // TODO: Route this to external clients
}

void IPCServer::handleMessage_GMCallResponse(const IPP& ipp, const ipc::GMCallResponse& message)
{
    TracyZoneScoped;

    // Client can only read up to 1024 bytes, drop any extra bytes now.
    const auto truncatedMessage = world::gmcall::TruncateResponse(message);

    world::gmcall::PersistResponse(truncatedMessage.callId, truncatedMessage.message);

    if (const auto maybeCharIPP = getIPPForCharId(truncatedMessage.charId))
    {
        sendMessage(*maybeCharIPP, truncatedMessage);
    }
}

void IPCServer::handleUnknownMessage(const IPP& ipp, const std::span<uint8_t> message)
{
    TracyZoneScoped;

    ShowWarningFmt("Received unknown message from {} with code {} and size {}", ipp.toString(), message[0], message.size());
}
