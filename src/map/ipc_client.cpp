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

#include "ipc_client.h"

#include "account_login.h"
#include "assist_channel_event.h"
#include "char_var_update.h"
#include "char_zone.h"
#include "chat_message_custom.h"
#include "chat_message_tell.h"
#include "entity_information_request.h"
#include "gmcall_request.h"
#include "group_chat_delivery.h"
#include "ipc_diagnostics.h"
#include "kill_session.h"
#include "linkshell_updates.h"
#include "lua_function.h"
#include "party_alliance_updates.h"
#include "party_invite.h"
#include "party_invite_response.h"
#include "player_kick_refresh.h"
#include "player_relocation.h"
#include "regional_event_dispatch.h"
#include "standard_message_delivery.h"
#include "zone_wide_chat_delivery.h"

#include "common/ipp.h"

#include <concurrentqueue.h>
#include <queue>

#include "alliance.h"
#include "aman.h"
#include "conquest_system.h"
#include "linkshell.h"
#include "map_networking.h"
#include "party.h"
#include "status_effect_container.h"
#include "unitychat.h"

#include "entities/char_entity.h"

#include "lua/luautils.h"

#include "packets/s2c/0x009_message.h"
#include "packets/s2c/0x017_chat_std.h"
#include "packets/s2c/0x053_systemmes.h"
#include "packets/s2c/0x0cc_linkshell_message.h"
#include "packets/s2c/0x0dc_group_solicit_req.h"

#include "gmcall_container.h"
#include "gmcall_response_notification.h"
#include "items/item_linkshell.h"
#include "utils/charutils.h"
#include "utils/jailutils.h"
#include "utils/serverutils.h"
#include "utils/zoneutils.h"

namespace
{

IPCClient* sClient = nullptr;

} // namespace

void message::init(IPCClient& client)
{
    sClient = &client;
}

auto message::detail::client() -> IPCClient&
{
    return *sClient;
}

auto message::detail::initialized() -> bool
{
    return sClient != nullptr;
}

void message::handle_incoming()
{
    TracyZoneScoped;

    sClient->handleIncomingMessages();
}

IPCClient::IPCClient(MapNetworking& networking, ZMQService& zmqService)
: networking_(networking)
, channel_(zmqService.registerDealer(getZMQEndpointString(), getZMQRoutingId()))
{
    TracyZoneScoped;
}

IPCClient::~IPCClient()
{
    if (sClient == this)
    {
        sClient = nullptr;
    }
}

auto IPCClient::getZMQEndpointString() -> std::string
{
    return fmt::format(
        "{}://{}:{}",
        settings::get<std::string>("network.ZMQ_TRANSPORT"),
        settings::get<std::string>("network.ZMQ_IP"),
        settings::get<uint16>("network.ZMQ_PORT"));
}

auto IPCClient::getZMQRoutingId() -> uint64
{
    auto ip   = networking_.ipp().getIP();
    auto port = networking_.ipp().getPort();

    // if no ip/port were supplied, set to 1 (0 is not valid for an identity)
    if (ip == 0 && port == 0)
    {
        const auto rset = db::preparedStmt("SELECT zoneip, zoneport FROM zone_settings GROUP BY zoneip, zoneport ORDER BY COUNT(*) DESC");
        if (rset && rset->rowsCount() && rset->next())
        {
            ip   = str2ip(rset->get<std::string>("zoneip"));
            port = rset->get<uint16>("zoneport");
        }
    }

    auto ipp = IPP(ip, port).getRawIPP();
    if (ipp == 0)
    {
        ShowWarning("ZMQ Routing ID IPP calculated as 0 - setting to 1. Check your zone_settings!");
        ipp = 1;
    }

    return ipp;
}

void IPCClient::handleIncomingMessages()
{
    TracyZoneScoped;

    // TODO: Can we stop more messages appearing on the queue while we're processing?
    zmq::message_t out;
    while (channel_.tryReceive(out))
    {
        const auto firstByte = out.data<uint8>()[0];
        const auto msgType   = ipc::toString(static_cast<ipc::MessageType>(firstByte));

        LogWith({ "ipc_msg", msgType });

        // TODO: Make an IPP for the world server, so we can use it here
        DebugIPCFmt("Incoming {} message", msgType);

        handleMessage(IPP(), { static_cast<uint8*>(out.data()), out.size() });
    }
}

void IPCClient::handleMessage_EmptyStruct(const IPP& ipp, const ipc::EmptyStruct& message)
{
    TracyZoneScoped;

    ShowWarning(mapipc::FormatEmptyStructWarning(ipp));
}

void IPCClient::handleMessage_AccountLogin(const IPP& ipp, const ipc::AccountLogin& message)
{
    TracyZoneScoped;

    mapipc::HandleAccountLogin(
        message,
        [this](const uint32 accountId)
        {
            return networking_.sessions().getSessionByAccountId(accountId);
        },
        []
        {
            return xirand::GetRandomNumber<uint32_t>(std::numeric_limits<uint32_t>::max());
        },
        []
        {
            return xirand::GetRandomNumber<uint16_t>(std::numeric_limits<uint16_t>::max());
        });
}

void IPCClient::handleMessage_CharZone(const IPP& ipp, const ipc::CharZone& message)
{
    TracyZoneScoped;

    mapipc::HandleCharZone(
        message,
        [this](const uint32 charId)
        {
            return networking_.sessions().getSessionByCharId(charId);
        },
        [](MapSession* session)
        {
            session->tapLastUpdate();
        },
        [this](const uint32 charId)
        {
            networking_.sessions().createPendingSession(charId);
        });
}

void IPCClient::handleMessage_CharVarUpdate(const IPP& ipp, const ipc::CharVarUpdate& message)
{
    TracyZoneScoped;

    mapipc::HandleCharVarUpdate(
        message,
        [](const uint32 charId)
        {
            return zoneutils::GetChar(charId);
        },
        [](CCharEntity* character, const std::string& varName, const int32 value, const uint32 expiry)
        {
            character->updateCharVarCache(varName, value, expiry);
        });
}

void IPCClient::handleMessage_ChatMessageTell(const IPP& ipp, const ipc::ChatMessageTell& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageTell(
        message,
        [](const std::string& recipientName)
        {
            return zoneutils::GetCharByName(recipientName);
        },
        [](CCharEntity* character)
        {
            return mapipc::InspectChatMessageTellRecipient(
                character->status == STATUS_TYPE::DISAPPEAR,
                [character]
                {
                    return jailutils::InPrison(character);
                },
                character->m_isGMHidden);
        },
        []
        {
            return settings::get<bool>("map.BLOCK_TELL_TO_HIDDEN_GM");
        },
        [](CCharEntity* character)
        {
            return character->isAway();
        },
        [](const ipc::MessageStandard& feedback)
        {
            message::send(feedback);
        },
        [](CCharEntity* character, const ipc::ChatMessageTell& tell)
        {
            character->pushPacket(std::make_unique<GP_SERV_COMMAND_CHAT_STD>(character, MESSAGE_TELL, tell.message, tell.senderName));
        });
}

void IPCClient::handleMessage_ChatMessageParty(const IPP& ipp, const ipc::ChatMessageParty& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageParty(
        message,
        [](const uint32 partyId)
        {
            CParty* party = nullptr;

            // TODO: When Party/Alliance gets a rewrite, make a zoneutils::ForEachParty or some other accessor to reduce the amount of iterations significantly.
            zoneutils::ForEachZone([partyId, &party](CZone* zone)
                                   {
                                       zone->ForEachChar([partyId, &party](CCharEntity* player)
                                                         {
                                                             if (player->PParty && player->PParty->GetPartyID() == partyId)
                                                             {
                                                                 party = player->PParty;
                                                             }
                                                         });
                                       if (party)
                                       {
                                           return;
                                       }
                                   });
            return party;
        },
        [](CParty* party, const ipc::ChatMessageParty& chat)
        {
            party->PushPacket(chat.senderId, 0, std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, chat.messageType, chat.message, chat.gmLevel));
        });
}

void IPCClient::handleMessage_ChatMessageAlliance(const IPP& ipp, const ipc::ChatMessageAlliance& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageAlliance(
        message,
        [](const uint32 allianceId)
        {
            CAlliance* alliance = nullptr;

            // TODO: When Party/Alliance gets a rewrite, make a zoneutils::ForEachParty or some other accessor to reduce the amount of iterations significantly.
            zoneutils::ForEachZone([allianceId, &alliance](CZone* zone)
                                   {
                                       zone->ForEachChar([allianceId, &alliance](CCharEntity* player)
                                                         {
                                                             if (player->PParty && player->PParty->m_PAlliance && player->PParty->m_PAlliance->m_AllianceID == allianceId)
                                                             {
                                                                 alliance = player->PParty->m_PAlliance;
                                                             }
                                                         });
                                       if (alliance)
                                       {
                                           return;
                                       }
                                   });
            return alliance;
        },
        [](CAlliance* alliance, const ipc::ChatMessageAlliance& chat)
        {
            for (const auto& currentParty : alliance->partyList)
            {
                currentParty->PushPacket(chat.senderId, 0, std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, chat.messageType, chat.message, chat.gmLevel));
            }
        });
}

void IPCClient::handleMessage_ChatMessageLinkshell(const IPP& ipp, const ipc::ChatMessageLinkshell& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageLinkshell(
        message,
        linkshell::GetLinkshell,
        [](CLinkshell* linkshell, const ipc::ChatMessageLinkshell& chat, const CHAT_MESSAGE_TYPE messageType)
        {
            // TODO: Linkshell 1 vs 2?
            linkshell->PushPacket(chat.senderId, std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, messageType, chat.message, chat.gmLevel));
        });
}

void IPCClient::handleMessage_ChatMessageUnity(const IPP& ipp, const ipc::ChatMessageUnity& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageUnity(
        message,
        unitychat::GetUnityChat,
        [](CUnityChat* unityChat, const ipc::ChatMessageUnity& chat)
        {
            unityChat->PushPacket(chat.senderId, std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, chat.messageType, chat.message, chat.gmLevel));
        });
}

void IPCClient::handleMessage_ChatMessageYell(const IPP& ipp, const ipc::ChatMessageYell& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageYell(
        message,
        [](auto&& visit)
        {
            zoneutils::ForEachZone([&](CZone* zone)
                                   {
                                       const bool zoneAllowsYell = zone->CanUseMisc(MISC_YELL);
                                       zone->ForEachChar([&](CCharEntity* character)
                                                         {
                                                             std::invoke(visit, zoneAllowsYell, character->id, character);
                                                         });
                                   });
        },
        [](CCharEntity* character, const ipc::ChatMessageYell& chat)
        {
            character->pushPacket(std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, chat.messageType, chat.message, chat.gmLevel));
        });
}

void IPCClient::handleMessage_ChatMessageAssist(const IPP& ipp, const ipc::ChatMessageAssist& message) const
{
    TracyZoneScoped;

    mapipc::HandleChatMessageAssist(
        message,
        [](auto&& visit)
        {
            zoneutils::ForEachZone([&](CZone* zone)
                                   {
                                       const bool zoneAllowsAssist = zone->CanUseMisc(MISC_ASSIST);
                                       zone->ForEachChar([&](CCharEntity* character)
                                                         {
                                                             std::invoke(visit, zoneAllowsAssist, character->id, character->aman().isAssistChannelEligible(), character);
                                                         });
                                   });
        },
        [](CCharEntity* character, const ipc::ChatMessageAssist& chat)
        {
            character->pushPacket(std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat));
        });
}

void IPCClient::handleMessage_ChatMessageServerMessage(const IPP& ipp, const ipc::ChatMessageServerMessage& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageServerMessage(
        message,
        [](auto&& visit)
        {
            zoneutils::ForEachZone([&](CZone* zone)
                                   {
                                       zone->ForEachChar([&](CCharEntity* character)
                                                         {
                                                             std::invoke(visit, character->id, character);
                                                         });
                                   });
        },
        [](CCharEntity* character, const ipc::ChatMessageServerMessage& chat)
        {
            character->pushPacket(std::make_unique<GP_SERV_COMMAND_CHAT_STD>(chat.senderName, chat.zoneId, chat.messageType, chat.message, chat.gmLevel));
        });
}

void IPCClient::handleMessage_ChatMessageCustom(const IPP& ipp, const ipc::ChatMessageCustom& message)
{
    TracyZoneScoped;

    mapipc::HandleChatMessageCustom(
        message,
        zoneutils::GetChar,
        [](CCharEntity* character)
        {
            return std::pair{ character->status == STATUS_TYPE::DISAPPEAR, jailutils::InPrison(character) };
        },
        [](CCharEntity* character, const ipc::ChatMessageCustom& chat)
        {
            character->pushPacket(std::make_unique<GP_SERV_COMMAND_CHAT_STD>(character, chat.messageType, chat.message, chat.senderName));
        });
}

void IPCClient::handleMessage_PartyInvite(const IPP& ipp, const ipc::PartyInvite& message)
{
    TracyZoneScoped;

    mapipc::HandlePartyInvite(
        message,
        zoneutils::GetChar,
        [](CCharEntity* invitee)
        {
            return mapipc::PartyInviteeSnapshot{
                .dead          = invitee->isDead(),
                .inPrison      = jailutils::InPrison(invitee),
                .invitePending = invitee->InvitePending.id != 0,
                .hasParty      = invitee->PParty != nullptr,
                .isPartyLeader = invitee->PParty && invitee->PParty->GetLeader() == invitee,
                .hasAlliance   = invitee->PParty && invitee->PParty->m_PAlliance,
                .blockingAid   = invitee->getBlockingAid(),
                .hasLevelSync  = invitee->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::LevelSync),
            };
        },
        [](const ipc::MessageStandard& feedback)
        {
            message::send(feedback);
        },
        [](const ipc::MessageSystem& feedback)
        {
            message::send(feedback);
        },
        [](CCharEntity* invitee)
        {
            invitee->pushPacket<GP_SERV_COMMAND_SYSTEMMES>(0, 0, MsgStd::BlockedByBlockaid);
        },
        [](CCharEntity* invitee, const ipc::PartyInvite& invite)
        {
            invitee->InvitePending.id     = invite.inviterId;
            invitee->InvitePending.targid = invite.inviterTargId;
            invitee->pushPacket(std::make_unique<GP_SERV_COMMAND_GROUP_SOLICIT_REQ>(invite.inviterId, invite.inviterTargId, invite.inviterName, invite.inviteType));
        });
}

void IPCClient::handleMessage_PartyInviteResponse(const IPP& ipp, const ipc::PartyInviteResponse& message)
{
    TracyZoneScoped;

    mapipc::HandlePartyInviteResponse(
        message,
        zoneutils::GetChar,
        [](CCharEntity* inviter)
        {
            return mapipc::PartyInviteResponseInviterState{
                .hasParty      = inviter->PParty != nullptr,
                .isPartyLeader = inviter->PParty && inviter->PParty->GetLeader() == inviter,
                .hasAlliance   = inviter->PParty && inviter->PParty->m_PAlliance,
            };
        },
        [](const uint32 inviterId, const uint32 inviteeId) -> std::size_t
        {
            // both party leaders?
            const auto rset = db::preparedStmt("SELECT * FROM accounts_parties WHERE partyid <> 0 AND "
                                               "((charid = ? OR charid = ?) AND partyflag & ?)",
                                               inviterId,
                                               inviteeId,
                                               PARTY_LEADER);
            return rset ? rset->rowsCount() : 0;
        },
        [](const uint32 inviterId) -> std::size_t
        {
            const auto rset = db::preparedStmt("SELECT * FROM accounts_parties WHERE allianceid <> 0 AND "
                                               "allianceid = (SELECT allianceid FROM accounts_parties where "
                                               "charid = ?) GROUP BY partyid",
                                               inviterId);
            return rset ? rset->rowsCount() : 0;
        },
        [](const uint32 inviteeId) -> bool
        {
            const auto rset = db::preparedStmt("SELECT * FROM accounts_parties WHERE partyid <> 0 AND charid = ?", inviteeId);
            return rset && rset->rowsCount() != 0;
        },
        [](CCharEntity* inviter)
        {
            inviter->pushPacket<GP_SERV_COMMAND_MESSAGE>(inviter, 0, 0, MsgStd::InvitationDeclined);
        },
        [](CCharEntity* inviter, const uint32 inviteeId)
        {
            inviter->PParty->m_PAlliance->addParty(inviteeId);
        },
        [](CCharEntity* inviter, const uint32 inviteeId)
        {
            CAlliance* alliance = new CAlliance(inviter);
            alliance->addParty(inviteeId);
        },
        [](CCharEntity* inviter)
        {
            if (inviter->PParty == nullptr)
            {
                inviter->PParty = new CParty(inviter);
            }
        },
        [](CCharEntity* inviter, const uint32 inviteeId)
        {
            inviter->PParty->AddMember(inviteeId);
        },
        [](const ipc::MessageStandard& feedback)
        {
            message::send(feedback);
        });
}

void IPCClient::handleMessage_PartyReload(const IPP& ipp, const ipc::PartyReload& message)
{
    TracyZoneScoped;

    mapipc::HandlePartyReload(
        message,
        [](const uint32 partyId, auto&& visit)
        {
            const auto rset = db::preparedStmt("SELECT charid FROM accounts_parties WHERE partyid = ?", partyId);
            if (rset && rset->rowsCount())
            {
                while (rset->next())
                {
                    std::invoke(visit, rset->get<uint32>("charid"));
                }
            }
        },
        [](const uint32 characterId)
        {
            return zoneutils::GetChar(characterId);
        });
}

void IPCClient::handleMessage_PartyDisband(const IPP& ipp, const ipc::PartyDisband& message)
{
    TracyZoneScoped;

    mapipc::HandlePartyDisband(
        message,
        [](const uint32 partyId)
        {
            CParty* party = nullptr;

            // TODO: Add a zoneutils::ForEachParty accessor when party/alliance is rewritten.
            zoneutils::ForEachZone([partyId, &party](CZone* zone)
                                   {
                                       zone->ForEachChar([partyId, &party](CCharEntity* player)
                                                         {
                                                             if (player->PParty && player->PParty->GetPartyID() == partyId)
                                                             {
                                                                 party = player->PParty;
                                                             }
                                                         });
                                   });
            return party;
        });
}

void IPCClient::handleMessage_AllianceReload(const IPP& ipp, const ipc::AllianceReload& message)
{
    TracyZoneScoped;

    mapipc::HandleAllianceReload(
        message,
        [](const uint32 allianceId, auto&& visit)
        {
            const auto rset = db::preparedStmt("SELECT charid FROM accounts_parties WHERE allianceid = ?", allianceId);
            if (rset && rset->rowsCount())
            {
                while (rset->next())
                {
                    std::invoke(visit, rset->get<uint32>("charid"));
                }
            }
        },
        [](const uint32 characterId)
        {
            return zoneutils::GetChar(characterId);
        });
}

void IPCClient::handleMessage_AllianceDissolve(const IPP& ipp, const ipc::AllianceDissolve& message)
{
    TracyZoneScoped;

    mapipc::HandleAllianceDissolve(
        message,
        [](const uint32 allianceId)
        {
            CAlliance* alliance = nullptr;

            // TODO: Add a zoneutils::ForEachAlliance accessor when party/alliance is rewritten.
            zoneutils::ForEachZone([allianceId, &alliance](CZone* zone)
                                   {
                                       zone->ForEachChar([allianceId, &alliance](CCharEntity* player)
                                                         {
                                                             if (player->PParty && player->PParty->m_PAlliance &&
                                                                 player->PParty->m_PAlliance->m_AllianceID == allianceId)
                                                             {
                                                                 alliance = player->PParty->m_PAlliance;
                                                             }
                                                         });
                                   });
            return alliance;
        });
}

void IPCClient::handleMessage_PlayerKick(const IPP& ipp, const ipc::PlayerKick& message)
{
    TracyZoneScoped;

    mapipc::HandlePlayerKick(message, [](const uint32 victimId)
                             {
                                 return zoneutils::GetChar(victimId);
                             });
}

void IPCClient::handleMessage_MessageStandard(const IPP& ipp, const ipc::MessageStandard& message)
{
    TracyZoneScoped;

    mapipc::HandleMessageStandard(
        message,
        [](const uint32 recipientId)
        {
            return zoneutils::GetChar(recipientId);
        },
        [](CCharEntity* player, const std::string& string2, const MsgStd messageId)
        {
            // TODO: Exchange the packet struct over IPC to avoid having to match one-offs.
            player->pushPacket(std::make_unique<GP_SERV_COMMAND_MESSAGE>(string2, messageId));
        },
        [](CCharEntity* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            player->pushPacket(std::make_unique<GP_SERV_COMMAND_MESSAGE>(player, param0, param1, messageId));
        });
}

void IPCClient::handleMessage_MessageSystem(const IPP& ipp, const ipc::MessageSystem& message)
{
    TracyZoneScoped;

    mapipc::HandleMessageSystem(
        message,
        [](const uint32 recipientId)
        {
            return zoneutils::GetChar(recipientId);
        },
        [](CCharEntity* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            player->pushPacket(std::make_unique<GP_SERV_COMMAND_MESSAGE>(player, param0, param1, messageId));
        });
}

void IPCClient::handleMessage_LinkshellRankChange(const IPP& ipp, const ipc::LinkshellRankChange& message)
{
    TracyZoneScoped;

    mapipc::HandleLinkshellRankChange(message, linkshell::GetLinkshell);
}

void IPCClient::handleMessage_LinkshellRemove(const IPP& ipp, const ipc::LinkshellRemove& message)
{
    TracyZoneScoped;

    mapipc::HandleLinkshellRemove(
        message,
        zoneutils::GetCharByName,
        [](CCharEntity* character, const int slot)
        {
            return slot == 1 ? character->PLinkshell1 : character->PLinkshell2;
        },
        [](CCharEntity* character, const int slot) -> std::optional<uint8>
        {
            auto* item = reinterpret_cast<CItemLinkshell*>(character->getEquip(slot == 1 ? SLOT_LINK1 : SLOT_LINK2));
            return item ? std::optional<uint8>{ static_cast<uint8>(item->GetLSType()) } : std::nullopt;
        });
}

void IPCClient::handleMessage_LinkshellSetMessage(const IPP& ipp, const ipc::LinkshellSetMessage& message)
{
    TracyZoneScoped;

    mapipc::HandleLinkshellSetMessage(
        message,
        linkshell::GetLinkshell,
        [](CLinkshell* linkshell, const ipc::LinkshellSetMessage& update)
        {
            linkshell->PushPacket(0, std::make_unique<GP_SERV_COMMAND_LINKSHELL_MESSAGE>(update.poster, update.message, update.linkshellName, update.postTime, LinkshellSlot::LS1));
        });
}

void IPCClient::handleMessage_LuaFunction(const IPP& ipp, const ipc::LuaFunction& message)
{
    TracyZoneScoped;

    mapipc::HandleLuaFunction(
        message,
        [](const std::string& funcString) -> std::optional<std::string>
        {
            auto result = lua.safe_script(funcString);
            if (result.valid())
            {
                // TODO: Handle a return value from result, and send back to message.requesterZoneId
                return std::nullopt;
            }
            sol::error err = result;
            return std::string{ err.what() };
        },
        [](const std::string& line)
        {
            ShowError("%s", line.c_str());
        });
}

void IPCClient::handleMessage_KillSession(const IPP& ipp, const ipc::KillSession& message)
{
    TracyZoneScoped;

    mapipc::HandleKillSession(
        message,
        [this](const uint32 victimId)
        {
            return networking_.sessions().getSessionByCharId(victimId);
        },
        [this](const uint32 victimId)
        {
            return networking_.sessions().getPendingSessionByCharId(victimId);
        },
        [](auto* session)
        {
            return session->blowfish.status;
        },
        [this, &message](auto* session)
        {
            ShowDebugFmt("Closing session of charid {} on request of other process", message.victimId);
            networking_.sessions().destroySession(session);
        },
        [this, &message](auto* session)
        {
            ShowDebugFmt("Closing pending session of charid {} on request of other process", message.victimId);
            networking_.sessions().destroyPendingSession(session);
        });
}

void IPCClient::handleMessage_ConquestEvent(const IPP& ipp, const ipc::ConquestEvent& message)
{
    TracyZoneScoped;

    mapipc::HandleConquestEvent(
        message,
        [](const ConquestMessage type, const std::span<const uint8> payload)
        {
            conquest::HandleMessage(type, payload);
        });
}

void IPCClient::handleMessage_BesiegedEvent(const IPP& ipp, const ipc::BesiegedEvent& message)
{
    TracyZoneScoped;

    mapipc::HandleBesiegedEvent(message);
}

void IPCClient::handleMessage_CampaignEvent(const IPP& ipp, const ipc::CampaignEvent& message)
{
    TracyZoneScoped;

    mapipc::HandleCampaignEvent(message);
}

void IPCClient::handleMessage_ColonizationEvent(const IPP& ipp, const ipc::ColonizationEvent& message)
{
    TracyZoneScoped;

    mapipc::HandleColonizationEvent(message);
}

void IPCClient::handleMessage_EntityInformationRequest(const IPP& ipp, const ipc::EntityInformationRequest& message)
{
    TracyZoneScoped;

    const auto snapshot = [](CBaseEntity* entity) -> std::optional<mapipc::EntityInformationTarget>
    {
        if (!entity)
        {
            return std::nullopt;
        }
        return mapipc::EntityInformationTarget{
            .id         = entity->id,
            .objType    = entity->objtype,
            .status     = entity->status,
            .hasZone    = entity->loc.zone != nullptr,
            .zoneId     = static_cast<uint16>(entity->loc.zone ? entity->loc.zone->GetID() : 0),
            .x          = entity->loc.p.x,
            .y          = entity->loc.p.y,
            .z          = entity->loc.p.z,
            .rot        = entity->loc.p.rotation,
            .moghouseId = entity->objtype == TYPE_PC ? static_cast<CCharEntity*>(entity)->m_moghouseID : 0,
        };
    };

    mapipc::HandleEntityInformationRequest(
        message,
        [&](const uint32 targetId)
        {
            return snapshot(zoneutils::GetChar(targetId));
        },
        [&](const uint32 targetId)
        {
            return snapshot(zoneutils::GetEntity(targetId));
        },
        [](const uint32 entityId) -> std::optional<mapipc::EntityInformationPosition>
        {
            // If a mob is not spawned, use its last database spawn point.
            const auto rset = db::preparedStmt("SELECT pos_x, pos_y, pos_z FROM mob_spawn_points WHERE mobid = ?", entityId);
            if (!rset || !rset->rowsCount())
            {
                return std::nullopt;
            }

            mapipc::EntityInformationPosition position{};
            while (rset->next())
            {
                position.x = rset->get<float>("pos_x");
                position.y = rset->get<float>("pos_y");
                position.z = rset->get<float>("pos_z");
            }
            return position;
        },
        [](const ipc::EntityInformationResponse& response)
        {
            message::send(response);
        },
        [](const uint32 targetId)
        {
            ShowWarningFmt("EntityInformationRequest for entity {} failed", targetId);
        });
}

void IPCClient::handleMessage_EntityInformationResponse(const IPP& ipp, const ipc::EntityInformationResponse& message)
{
    TracyZoneScoped;

    mapipc::HandleEntityInformationResponse(message, [](const uint32 requesterId)
                                            {
                                                return zoneutils::GetChar(requesterId);
                                            });
}

void IPCClient::handleMessage_SendPlayerToLocation(const IPP& ipp, const ipc::SendPlayerToLocation& message)
{
    TracyZoneScoped;

    mapipc::HandleSendPlayerToLocation(message, [](const uint32 targetId)
                                       {
                                           return zoneutils::GetChar(targetId);
                                       });
}

void IPCClient::handleMessage_AssistChannelEvent(const IPP& ipp, const ipc::AssistChannelEvent& message) const
{
    TracyZoneScoped;

    mapipc::HandleAssistChannelEvent(message, [](const uint32 receiverId)
                                     {
                                         return zoneutils::GetChar(receiverId);
                                     });
}

void IPCClient::handleMessage_GMCallRequest(const IPP& ipp, const ipc::GMCallRequest& message)
{
    TracyZoneScoped;

    mapipc::HandleGMCallRequest(ipp, message);
}

void IPCClient::handleMessage_GMCallResponse(const IPP& ipp, const ipc::GMCallResponse& message)
{
    TracyZoneScoped;

    mapipc::HandleGMCallResponse(
        message,
        [](const uint32 charId)
        {
            return zoneutils::GetChar(charId);
        },
        [](CCharEntity* PChar)
        {
            PChar->gmCallContainer().sendPendingResponse(PChar);
        });
}

void IPCClient::handleUnknownMessage(const IPP& ipp, const std::span<uint8_t> message)
{
    TracyZoneScoped;

    ShowWarning(mapipc::FormatUnknownMessageWarning(ipp, message[0], message.size()));
}
