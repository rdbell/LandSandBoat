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

#include "character_name.h"
#include "data_session.h"
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
        ShowWarning(fmt::format("Session requested without valid sessionHash from {}", ipAddress));
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
            const auto requestedCharacterID                 = ref<uint32>(buffer_.data(), 28);
            char       requestedCharacter[PacketNameLength] = {};
            std::memcpy(&requestedCharacter, buffer_.data() + 36, PacketNameLength - 1);

            uint32 accountID = 0;

            const auto rset = db::preparedStmt("SELECT accid FROM chars WHERE charid = ? AND charname = ? LIMIT 1",
                                               requestedCharacterID,
                                               requestedCharacter);
            if (rset && rset->rowsCount() != 0 && rset->next())
            {
                accountID                    = rset->get<uint32>("accid");
                session.requestedCharacterID = requestedCharacterID;
            }
            else
            {
                ShowError(fmt::format("Account ID {} tried to select a character id with a mismatched character name.", session.accountID));
                socket_.lowest_layer().close();
                return;
            }

            if (accountID != session.accountID)
            {
                ShowError(fmt::format("Account ID {} tried to login as character not in their account.", session.accountID));
                socket_.lowest_layer().close();
                return;
            }

            if (auto data = session.data_session)
            {
                std::memset(data->buffer_.data(), 0, 0x05);
                data->buffer_.data()[0] = 0x02;
                data->do_write(0x05);
            }
        }
        break;
        case 0x14: // 20: "Deleting from lobby server"
        {
            if (loginHelpers::ClassifyCharacterDeletionGate(settings::get<bool>("login.CHARACTER_DELETION")) ==
                loginHelpers::character_deletion_gate::DENIED)
            {
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::COULD_NOT_CONNECT_TO_LOBBY_SERVER);
                do_write(0x24);
                return;
            }

            lpkt_deletechr deleteCharPacket = {};
            std::memcpy(&deleteCharPacket, buffer_.data(), sizeof(lpkt_deletechr));

            loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
            do_write(loginHelpers::ViewLobbyAckPacketSize);

            uint32 charID = deleteCharPacket.ffxi_id;

            ShowInfo(fmt::format("attempt to delete char:<{}> from ip:<{}>",
                                 charID,
                                 ipAddress));

            uint32 accountID = 0;

            const auto rset = db::preparedStmt("SELECT accid FROM chars WHERE charid = ? LIMIT 1", charID);
            if (rset && rset->rowsCount() != 0 && rset->next())
            {
                accountID = rset->get<uint32>("accid");
            }

            if (accountID != session.accountID)
            {
                ShowError(fmt::format("Account ID {} tried to delete character not in their account.", session.accountID));
                socket_.lowest_layer().close();
                return;
            }

            if (auto data = dynamic_cast<data_session*>(session.data_session.get()))
            {
                data->deleteCharFromCharInfo(charID);
            }

            // Perform character deletion.
            // Instead of performing an actual character deletion, we simply set accid to 0, and original_accid to old accid.
            // This allows character recovery.

            db::preparedStmt("UPDATE chars SET accid = 0, original_accid = ? WHERE charid = ? AND accid = ?",
                             session.accountID,
                             charID,
                             session.accountID);

            // Increment key after delete
            session.incrementKeyValue += loginHelpers::DeleteKeyIncrement;
        }
        break;
        case 0x21: // 33: Registering character name onto the lobby server
        {
            lpkt_chr_info_sub2 charInfo = {};
            // creating new char
            if (loginHelpers::createCharacter(session, buffer_.data(), charInfo) == -1)
            {
                socket_.lowest_layer().close();
                return;
            }

            if (auto data = dynamic_cast<data_session*>(session.data_session.get()))
            {
                data->addCharIntoCharInfo(charInfo);
            }

            session.justCreatedNewChar = true;
            ShowInfo(fmt::format("char <{}> was successfully created on account {}", session.requestedNewCharacterName, session.accountID));

            loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
            do_write(loginHelpers::ViewLobbyAckPacketSize);
        }
        break;
        case 0x22: // 34: Checking name and Gold World Pass
        {
            // block creation of character if in maintenance mode or generally disabled
            const auto maintMode               = settings::get<uint8>("login.MAINT_MODE");
            const auto enableCharacterCreation = settings::get<bool>("login.CHARACTER_CREATION");
            if (loginHelpers::ClassifyCharacterCreationGate(maintMode, enableCharacterCreation) ==
                loginHelpers::character_creation_gate::DENIED)
            {
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::FAILED_TO_REGISTER_WITH_THE_NAME_SERVER);
                do_write(0x24);
                return;
            }
            else
            {
                // creating new char
                char CharName[PacketNameLength] = {};
                std::memcpy(CharName, buffer_.data() + 32, PacketNameLength - 1);

                std::string nameStr = loginHelpers::ExtractCharacterNameField(CharName);

                // Local pure checks first (alpha then length overwrite).
                Maybe<std::string> invalidNameReason = loginHelpers::ValidateCharacterNameLocal(nameStr);

                // Check if the name is already in use by another character
                const auto rset0 = db::preparedStmt("SELECT charname FROM chars WHERE charname LIKE ?", nameStr);
                if (!rset0)
                {
                    invalidNameReason = loginHelpers::CharacterNameEntityQueryFailedReason;
                }
                else if (rset0 && rset0->rowsCount() != 0)
                {
                    invalidNameReason = loginHelpers::CharacterNameAlreadyInUseReason;
                }

                // (optional) Check if the name is in use by NPC or Mob entities
                if (settings::get<bool>("login.DISABLE_MOB_NPC_CHAR_NAMES"))
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
                    if (!rset1)
                    {
                        invalidNameReason = loginHelpers::CharacterNameEntityQueryFailedNoPeriodReason;
                    }
                    else if (rset1->rowsCount() != 0)
                    {
                        invalidNameReason = loginHelpers::CharacterNameAlreadyInUseReason;
                    }
                }

                // TODO: Don't raw-access Lua like this outside of Lua helper code.
                // (optional) Check if the name contains any words on the bad word list
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
                        invalidNameReason = *banned;
                    }
                }

                if (invalidNameReason.has_value())
                {
                    ShowWarning(fmt::format("new character name error <{}>: {}", nameStr, *invalidNameReason));

                    // Send error code:
                    // The character name you entered is unavailable. Please choose another name.
                    // TODO: This message is displayed in Japanese, needs fixing.
                    loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::CHARACTER_NAME_UNAVAILABLE);
                    do_write(0x24);
                    return;
                }
                else
                {
                    // copy charname
                    session.requestedNewCharacterName = CharName;

                    loginHelpers::GenerateViewLobbyAckPacket(buffer_.data());
                    do_write(loginHelpers::ViewLobbyAckPacketSize);
                }
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

            if (versionFlow.responseLength == login::version_lock::ResponseLength::VersionError)
            {
                if (auto data = session.view_session.get())
                {
                    loginHelpers::generateErrorMessage(data->buffer_.data(), loginErrors::errorCode::GAMES_DATA_HAS_BEEN_UPDATED); // "The games data has been updated"
                    data->do_write(static_cast<std::size_t>(versionFlow.responseLength));
                    return;
                }
            }

            const auto packet = loginPackets::generateKeyPacket(
                loginHelpers::generateExpansionBitmask(),
                loginHelpers::generateFeatureBitmask());
            std::memcpy(buffer_.data(), &packet, sizeof(lpkt_key));
            DebugSockets("view_session: Sending version and expansions info to account %d", session.accountID);

            if (auto data = session.view_session.get())
            {
                std::memcpy(data->buffer_.data(), buffer_.data(), 0x28);
                data->do_write(static_cast<std::size_t>(versionFlow.responseLength));
            }
        }
        break;
        case 0x1F: // 31: "Acquiring Player Data"
        {
            if (auto data = session.data_session.get())
            {
                std::memset(data->buffer_.data(), 0, 5);
                data->buffer_.data()[0] = 0x01;
                data->do_write(0x05);
            }
            else
            {
                loginHelpers::generateErrorMessage(buffer_.data(), loginErrors::errorCode::COULD_NOT_CONNECT_TO_LOBBY_SERVER); // "Could not connect to lobby server.\nPlease check this title's news for announcements."
                do_write(0x24);                                                                                                // This used to error, but this case is probably not valid after sessionHash. // TODO: is this this else block still needed?
                return;
            }
        }
        break;
        case 0x24: // 36: "Acquiring FINAL FANTASY XI server data"
        {
            std::memset(buffer_.data(), 0, 0x40);
            const auto serverName = settings::get<std::string>("main.SERVER_NAME");

            lpkt_world_list worldList = {};
            worldList.terminator      = loginPackets::getTerminator();
            worldList.command         = 0x23;

            loginPackets::clearIdentifier(worldList);

            // Send client 1 world
            worldList.sumofworld = 0x01;

            // Setup world id 0x20 with the server name from settings
            worldList.world_name[0].no = 0x20;
            std::memcpy(worldList.world_name[0].name, serverName.c_str(), std::clamp<size_t>(serverName.length(), 0, 15));

            if (auto data = session.view_session.get())
            {
                worldList.packet_size = sizeof(packet_t) + sizeof(uint32_t) + sizeof(lpkt_world_name) * worldList.sumofworld;

                unsigned char Hash[16];
                md5(reinterpret_cast<uint8*>(&worldList), Hash, worldList.packet_size);
                loginPackets::copyHashIntoPacket(worldList, Hash);

                std::memcpy(data->buffer_.data(), &worldList, worldList.packet_size);
                data->do_write(worldList.packet_size);
            }
        }
        break;
    }
}

void view_session::handle_error(std::error_code ec, std::shared_ptr<handler_session> self)
{
    if (self->sessionHash != "")
    {
        auto& map = loginHelpers::getAuthenticatedSessions()[self->ipAddress];
        auto  it  = map.find(self->sessionHash);

        if (it != map.end())
        {
            session_t& session = it->second;
            if (session.view_session.get())
            {
                session.view_session = nullptr;
            }

            if (session.data_session == nullptr && session.view_session == nullptr)
            {
                // Remove entry if needs to be
                map.erase(it);

                // Remove IP from map if no entries remain
                auto& sessions = loginHelpers::getAuthenticatedSessions();
                if (auto outerIt = sessions.find(self->ipAddress); outerIt != sessions.end() && outerIt->second.empty())
                {
                    sessions.erase(outerIt);
                }
            }
        }
    }
}
