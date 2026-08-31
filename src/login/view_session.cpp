/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "view_session.h"

#include "view_version_response.h"
#include "view_world_list_response.h"
#include "view_acquire_player_response.h"
#include "view_character_delete_response.h"
#include "view_name_check_response.h"
#include "view_character_create_response.h"

#include "character_delete.h"
#include "character_name.h"
#include "character_select.h"
#include "data_session.h"
#include "session_cleanup.h"
#include "view_lobby_ack.h"

#include <common/lua.h>
#include <common/settings.h>
#include <common/utils.h>

#include "login_packets.h"
#include "version_lock.h"

#include <vector>

void view_session::read_func()
{
    const auto code = ref<uint8>(buffer_.data(), 8);

    const auto sessionHash = loginHelpers::getHashFromPacket(ipAddress, buffer_.data());
    if (sessionHash == "")
    {
        ShowWarning(loginHelpers::FormatMissingSessionHashWarning(ipAddress));
        return;
    }

    session_t& session = loginHelpers::get_authenticated_session(ipAddress, sessionHash);
    if (!session.view_session)
    {
        session.view_session = std::make_shared<view_session>(std::forward<asio::ssl::stream<asio::ip::tcp::socket>>(socket_), dealerChannel_);
    }
    session.view_session->sessionHash = sessionHash;

    DebugSockets(fmt::format("view code: {}", code));

    switch (code)
    {
        case 0x07: // 07: "Notifying lobby server of current selections."
        {
            const auto requestedCharacterID                 = ref<uint32>(buffer_.data(), loginHelpers::CharacterSelectIDOffset);
            char       requestedCharacter[PacketNameLength] = {};
            std::memcpy(&requestedCharacter, buffer_.data() + loginHelpers::CharacterSelectNameOffset, PacketNameLength - 1);

            uint32     accountID = 0;
            bool       rowFound  = false;
            const auto rset      = db::preparedStmt("SELECT accid FROM chars WHERE charid = ? AND charname = ? LIMIT 1",
                                               requestedCharacterID,
                                               requestedCharacter);
            if (rset && rset->rowsCount() != 0 && rset->next())
            {
                accountID = rset->get<uint32>("accid");
                rowFound  = true;
            }

            const auto selectGate = loginHelpers::ClassifyCharacterSelect(
                static_cast<bool>(rset),
                rowFound,
                accountID,
                session.accountID);
            const auto selectPlan = loginHelpers::PlanCharacterSelect(selectGate, session.data_session != nullptr);
            if (selectPlan.closeViewSession)
            {
                if (selectGate == loginHelpers::character_select_gate::MISMATCHED_NAME)
                {
                    ShowError(loginHelpers::FormatCharacterSelectMismatchedName(session.accountID));
                }
                else
                {
                    ShowError(loginHelpers::FormatCharacterSelectWrongAccount(session.accountID));
                }
                socket_.lowest_layer().close();
                return;
            }

            if (selectPlan.setRequestedCharacterID)
            {
                session.requestedCharacterID = requestedCharacterID;
            }

            if (selectPlan.notifyDataSession)
            {
                auto data = session.data_session;
                loginHelpers::GenerateDataSelectNotifyPacket(data->buffer_.data());
                data->do_write(loginHelpers::DataSelectNotifyPacketSize);
            }
        }
        break;
        case 0x14: // 20: "Deleting from lobby server"
        {
            const auto enablePlan = login::PlanViewCharacterDeleteEnableResponse(
                loginHelpers::ClassifyCharacterDeletionGate(settings::get<bool>("login.CHARACTER_DELETION")));
            if (enablePlan.writeLobbyError)
            {
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::COULD_NOT_CONNECT_TO_LOBBY_SERVER);
                do_write(0x24);
            }
            if (enablePlan.returnFromRead)
            {
                return;
            }

            lpkt_deletechr deleteCharPacket = {};
            std::memcpy(&deleteCharPacket, buffer_.data(), sizeof(lpkt_deletechr));
            uint32 charID = deleteCharPacket.ffxi_id;

            // Ack is sent before ownership checks (LSB order).
            if (enablePlan.writeLobbyAck)
            {
                loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
                do_write(loginHelpers::ViewLobbyAckPacketSize);
            }

            if (enablePlan.logDeleteAttempt)
            {
                ShowInfo(loginHelpers::FormatCharacterDeleteAttemptInfo(charID, ipAddress));
            }

            bool       rowFound = false;
            uint32     rowAccid = 0;
            if (enablePlan.runOwnershipQuery)
            {
                const auto rset = db::preparedStmt("SELECT accid FROM chars WHERE charid = ? LIMIT 1", charID);
                if (rset && rset->rowsCount() != 0 && rset->next())
                {
                    rowAccid = rset->get<uint32>("accid");
                    rowFound = true;
                }

                const uint32 lookedUpAccountID = loginHelpers::LookedUpAccountIDFromDeleteQuery(
                    static_cast<bool>(rset),
                    rowFound,
                    rowAccid);
                const auto ownershipPlan = login::PlanViewCharacterDeleteOwnershipResponse(
                    loginHelpers::ClassifyCharacterDeleteOwnership(lookedUpAccountID, session.accountID),
                    session.data_session != nullptr);

                if (ownershipPlan.logWrongAccount)
                {
                    ShowError(loginHelpers::FormatCharacterDeleteWrongAccount(session.accountID));
                }
                if (ownershipPlan.closeViewSocket)
                {
                    socket_.lowest_layer().close();
                }
                if (ownershipPlan.returnFromRead)
                {
                    return;
                }

                if (ownershipPlan.clearCharFromDataSession)
                {
                    if (auto data = dynamic_cast<data_session*>(session.data_session.get()))
                    {
                        data->deleteCharFromCharInfo(charID);
                    }
                }

                // Perform character deletion.
                // Instead of performing an actual character deletion, we simply set accid to 0, and original_accid to old accid.
                // This allows character recovery.
                if (ownershipPlan.softDeleteCharacter)
                {
                    db::preparedStmt("UPDATE chars SET accid = 0, original_accid = ? WHERE charid = ? AND accid = ?",
                                     session.accountID,
                                     charID,
                                     session.accountID);
                }

                // Increment key after delete
                if (ownershipPlan.bumpDeleteKey)
                {
                    session.incrementKeyValue += loginHelpers::DeleteKeyIncrement;
                }
            }
        }
        break;
        case 0x21: // 33: Registering character name onto the lobby server
        {
            lpkt_chr_info_sub2 charInfo = {};
            // creating new char
            const bool createSucceeded = loginHelpers::createCharacter(session, buffer_.data(), charInfo) != -1;
            const auto responsePlan    = login::PlanViewCharacterCreateResponse(
                createSucceeded,
                session.data_session != nullptr);
            if (responsePlan.closeViewSocket)
            {
                socket_.lowest_layer().close();
            }
            if (responsePlan.returnFromRead)
            {
                return;
            }
            if (responsePlan.addCharIntoDataSession)
            {
                if (auto data = dynamic_cast<data_session*>(session.data_session.get()))
                {
                    data->addCharIntoCharInfo(charInfo);
                }
            }
            if (responsePlan.setJustCreatedNewChar)
            {
                session.justCreatedNewChar = true;
            }
            if (responsePlan.logCharacterCreated)
            {
                ShowInfo(loginHelpers::FormatCharacterCreatedInfo(session.requestedNewCharacterName, session.accountID));
            }
            if (responsePlan.writeLobbyAck)
            {
                loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
                do_write(loginHelpers::ViewLobbyAckPacketSize);
            }
        }
        break;
        case 0x22: // 34: Checking name and Gold World Pass
        {
            // block creation of character if in maintenance mode or generally disabled
            const auto maintMode               = settings::get<uint8>("login.MAINT_MODE");
            const auto enableCharacterCreation = settings::get<bool>("login.CHARACTER_CREATION");
            const auto enablePlan              = login::PlanViewNameCheckEnableResponse(
                loginHelpers::ClassifyCharacterCreationGate(maintMode, enableCharacterCreation));
            if (enablePlan.writeRegisterError)
            {
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::FAILED_TO_REGISTER_WITH_THE_NAME_SERVER);
                do_write(0x24);
            }
            if (enablePlan.returnFromRead)
            {
                return;
            }

            // creating new char
            char CharName[PacketNameLength] = {};
            std::memcpy(CharName, buffer_.data() + 32, PacketNameLength - 1);

            std::string nameStr = loginHelpers::ExtractCharacterNameField(CharName);

            // Local pure checks first (alpha then length overwrite).
            Maybe<std::string> invalidNameReason = loginHelpers::ValidateCharacterNameLocal(nameStr);

            // Check if the name is already in use by another character
            const auto rset0 = db::preparedStmt("SELECT charname FROM chars WHERE charname LIKE ?", nameStr);
            const bool entityQueryOk  = static_cast<bool>(rset0);
            const bool entityNameTaken = entityQueryOk && rset0->rowsCount() != 0;

            // (optional) Check if the name is in use by NPC or Mob entities
            const bool checkMobNPCNames = settings::get<bool>("login.DISABLE_MOB_NPC_CHAR_NAMES");
            bool       mobNPCQueryOk   = true;
            bool       mobNPCNameTaken = false;
            if (checkMobNPCNames)
            {
                const auto query =
                    "SELECT polutils_name AS `name` FROM npc_list "
                    "WHERE REPLACE(REPLACE(UPPER(polutils_name), '-', ''), '_', '') "
                    "LIKE REPLACE(REPLACE(UPPER(?), '-', ''), '_', '') "
                    "UNION "
                    "SELECT packet_name AS `name` FROM mob_pools "
                    "WHERE REPLACE(REPLACE(UPPER(packet_name), '-', ''), '_', '') "
                    "LIKE REPLACE(REPLACE(UPPER(?), '-', ''), '_', '')";

                const auto rset1 = db::preparedStmt(query, nameStr, nameStr);
                mobNPCQueryOk   = static_cast<bool>(rset1);
                mobNPCNameTaken = mobNPCQueryOk && rset1->rowsCount() != 0;
            }

            // TODO: Don't raw-access Lua like this outside of Lua helper code.
            // (optional) Check if the name contains any words on the bad word list
            Maybe<std::string> bannedReason = std::nullopt;
            const auto loginSettingsTable = lua["xi"]["settings"]["login"].get<sol::table>();
            if (auto badWordsList = loginSettingsTable.get_or<sol::table>("BANNED_WORDS_LIST", sol::lua_nil); badWordsList.valid())
            {
                const auto              potentialName = to_upper(nameStr);
                std::vector<std::string> upperBadWords;
                for (const auto& entry : badWordsList)
                {
                    upperBadWords.push_back(to_upper(entry.second.as<std::string>()));
                }
                if (auto banned = loginHelpers::FindBannedWordMatch(potentialName, upperBadWords))
                {
                    bannedReason = *banned;
                }
            }

            invalidNameReason = loginHelpers::ResolveCharacterNameInvalidReason(
                std::move(invalidNameReason),
                entityQueryOk,
                entityNameTaken,
                checkMobNPCNames,
                mobNPCQueryOk,
                mobNPCNameTaken,
                std::move(bannedReason));

            const auto namePlan = login::PlanViewNameCheckNameResponse(!invalidNameReason.has_value());
            if (namePlan.logInvalidName)
            {
                ShowWarning(loginHelpers::FormatNewCharacterNameError(nameStr, *invalidNameReason));
            }
            if (namePlan.writeNameUnavailableError)
            {
                // Send error code:
                // The character name you entered is unavailable. Please choose another name.
                // TODO: This message is displayed in Japanese, needs fixing.
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::CHARACTER_NAME_UNAVAILABLE);
                do_write(0x24);
            }
            if (namePlan.returnFromRead)
            {
                return;
            }
            if (namePlan.setRequestedNewCharacterName)
            {
                // copy charname
                session.requestedNewCharacterName = CharName;
            }
            if (namePlan.writeLobbyAck)
            {
                loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
                do_write(loginHelpers::ViewLobbyAckPacketSize);
            }
        }
        break;
        case 0x26: // 38: Version + Expansions, "Setting up connection."
        {
            const auto versionFlow = login::version_lock::evaluateFlow(
                std::span<const uint8>(buffer_.data() + 0x74, login::version_lock::PrefixLength),
                settings::get<std::string>("login.CLIENT_VER"),
                []()
                {
                    return settings::get<uint8>("login.VER_LOCK");
                });
            const auto& versionDecision = versionFlow.version;
            const auto client_ver_data = versionDecision.clientVersion;
            DebugSockets(fmt::format("Version: {} from {}", client_ver_data, ipAddress));

            const auto expected_version = versionDecision.expectedVersion;
            const bool versionMismatch  = versionDecision.mismatch;

            if (versionMismatch)
            {
                ShowError(fmt::format("view_session: Account {} has incorrect client version: got {}, expected {}", session.accountID, client_ver_data, expected_version));

                switch (versionFlow.lockMode)
                {
                    // enabled
                    case login::version_lock::Strict:
                        if (versionDecision.direction == login::version_lock::MismatchDirection::ServerTooOld)
                        {
                            ShowError("view_session: The server must be updated to support this client version");
                        }
                        else
                        {
                            ShowError("view_session: The client must be updated to support this server version");
                        }
                        break;
                    // enabled greater than or equal
                    case login::version_lock::AllowNewer:
                        if (versionDecision.fatal)
                        {
                            ShowError("view_session: The client must be updated to support this server version");
                        }
                        break;
                    default:
                        // no-op - not enabled or unknown verlock type
                        break;
                }
            }

            const auto responsePlan = login::PlanViewVersionResponse(
                versionFlow.responseLength,
                session.view_session != nullptr);
            if (responsePlan.writeVersionError)
            {
                auto data = session.view_session.get();
                loginHelpers::generateErrorMessage(data->buffer_.data(), loginErrors::errorCode::GAMES_DATA_HAS_BEEN_UPDATED); // "The games data has been updated"
                data->do_write(static_cast<std::size_t>(versionFlow.responseLength));
            }
            if (responsePlan.returnFromRead)
            {
                return;
            }

            if (responsePlan.shapeKeyPacket)
            {
                const auto packet = loginPackets::generateKeyPacket(
                    loginHelpers::generateExpansionBitmask(),
                    loginHelpers::generateFeatureBitmask());
                std::memcpy(buffer_.data(), &packet, sizeof(lpkt_key));
                DebugSockets("view_session: Sending version and expansions info to account %d", session.accountID);
            }

            if (responsePlan.writeKeyPacket)
            {
                auto data = session.view_session.get();
                std::memcpy(data->buffer_.data(), buffer_.data(), 0x28);
                data->do_write(static_cast<std::size_t>(versionFlow.responseLength));
            }
        }
        break;
        case 0x1F: // 31: "Acquiring Player Data"
        {
            const auto presence = loginHelpers::ClassifyDataSessionPresence(session.data_session != nullptr);
            const auto responsePlan = login::PlanViewAcquirePlayerResponse(presence);
            if (responsePlan.writeAcquireNotify)
            {
                auto data = session.data_session.get();
                loginHelpers::GenerateDataAcquirePlayerNotifyPacket(data->buffer_.data());
                data->do_write(loginHelpers::DataSessionNotifyPacketSize);
            }
            if (responsePlan.writeLobbyError)
            {
                // "Could not connect to lobby server.\nPlease check this title's news for announcements."
                // This used to error, but this case is probably not valid after sessionHash.
                // TODO: is this this else block still needed?
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::COULD_NOT_CONNECT_TO_LOBBY_SERVER);
                do_write(0x24);
            }
            if (responsePlan.returnFromRead)
            {
                return;
            }
        }
        break;
        case 0x24: // 36: "Acquiring FINAL FANTASY XI server data"
        {
            const auto responsePlan = login::PlanViewWorldListResponse(session.view_session != nullptr);

            lpkt_world_list worldList = {};
            if (responsePlan.shapeWorldListPacket)
            {
                std::memset(buffer_.data(), 0, 0x40);
                const auto serverName = settings::get<std::string>("main.SERVER_NAME");

                worldList.terminator = loginPackets::getTerminator();
                worldList.command    = 0x23;

                loginPackets::clearIdentifier(worldList);

                // Send client 1 world
                worldList.sumofworld = 0x01;

                // Setup world id 0x20 with the server name from settings
                worldList.world_name[0].no = 0x20;
                std::memcpy(worldList.world_name[0].name, serverName.c_str(), std::clamp<size_t>(serverName.length(), 0, 15));

                worldList.packet_size = sizeof(packet_t) + sizeof(uint32_t) + sizeof(lpkt_world_name) * worldList.sumofworld;

                unsigned char Hash[16];
                md5(reinterpret_cast<uint8*>(&worldList), Hash, worldList.packet_size);
                loginPackets::copyHashIntoPacket(worldList, Hash);
            }

            if (responsePlan.writeWorldListPacket)
            {
                auto data = session.view_session.get();
                std::memcpy(data->buffer_.data(), &worldList, worldList.packet_size);
                data->do_write(worldList.packet_size);
            }
        }
        break;
    }
}

void view_session::handle_error(std::error_code ec, std::shared_ptr<handler_session> self)
{
    if (!loginHelpers::HasSessionHashForCleanup(self->sessionHash))
    {
        return;
    }

    auto* session = loginHelpers::find_authenticated_session(self->ipAddress, self->sessionHash);
    const bool entryFound = session != nullptr;
    const bool otherPeerPresent = entryFound && session->data_session != nullptr;
    const auto plan = loginHelpers::PlanSessionErrorCleanup(
        entryFound,
        loginHelpers::session_error_peer::VIEW,
        otherPeerPresent);

    if (plan.clearPeer)
    {
        session->view_session = nullptr;
    }

    if (plan.eraseSessionEntry)
    {
        loginHelpers::erase_authenticated_session(self->ipAddress, self->sessionHash);
    }
}
