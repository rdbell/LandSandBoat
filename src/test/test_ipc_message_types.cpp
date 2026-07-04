/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "test_ipc_message_types.h"

#include "common/ipc.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace
{

struct MessageTypeCase
{
    ipc::MessageType type;
    uint8_t          code;
    std::string_view name;
};

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC message type self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectEqualString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC message type self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto messageTypeCases() -> std::vector<MessageTypeCase>
{
    return {
        { ipc::MessageType::EmptyStruct, 1, "EmptyStruct" },
        { ipc::MessageType::AccountLogin, 2, "AccountLogin" },
        { ipc::MessageType::CharZone, 3, "CharZone" },
        { ipc::MessageType::CharVarUpdate, 4, "CharVarUpdate" },
        { ipc::MessageType::ChatMessageTell, 5, "ChatMessageTell" },
        { ipc::MessageType::ChatMessageParty, 6, "ChatMessageParty" },
        { ipc::MessageType::ChatMessageAlliance, 7, "ChatMessageAlliance" },
        { ipc::MessageType::ChatMessageLinkshell, 8, "ChatMessageLinkshell" },
        { ipc::MessageType::ChatMessageUnity, 9, "ChatMessageUnity" },
        { ipc::MessageType::ChatMessageYell, 10, "ChatMessageYell" },
        { ipc::MessageType::ChatMessageAssist, 11, "ChatMessageAssist" },
        { ipc::MessageType::ChatMessageServerMessage, 12, "ChatMessageServerMessage" },
        { ipc::MessageType::ChatMessageCustom, 13, "ChatMessageCustom" },
        { ipc::MessageType::PartyInvite, 14, "PartyInvite" },
        { ipc::MessageType::PartyInviteResponse, 15, "PartyInviteResponse" },
        { ipc::MessageType::PartyReload, 16, "PartyReload" },
        { ipc::MessageType::PartyDisband, 17, "PartyDisband" },
        { ipc::MessageType::AllianceReload, 18, "AllianceReload" },
        { ipc::MessageType::AllianceDissolve, 19, "AllianceDissolve" },
        { ipc::MessageType::PlayerKick, 20, "PlayerKick" },
        { ipc::MessageType::MessageStandard, 21, "MessageStandard" },
        { ipc::MessageType::MessageSystem, 22, "MessageSystem" },
        { ipc::MessageType::LinkshellRankChange, 23, "LinkshellRankChange" },
        { ipc::MessageType::LinkshellRemove, 24, "LinkshellRemove" },
        { ipc::MessageType::LinkshellSetMessage, 25, "LinkshellSetMessage" },
        { ipc::MessageType::LuaFunction, 26, "LuaFunction" },
        { ipc::MessageType::KillSession, 27, "KillSession" },
        { ipc::MessageType::ConquestEvent, 28, "ConquestEvent" },
        { ipc::MessageType::BesiegedEvent, 29, "BesiegedEvent" },
        { ipc::MessageType::CampaignEvent, 30, "CampaignEvent" },
        { ipc::MessageType::ColonizationEvent, 31, "ColonizationEvent" },
        { ipc::MessageType::EntityInformationRequest, 32, "EntityInformationRequest" },
        { ipc::MessageType::EntityInformationResponse, 33, "EntityInformationResponse" },
        { ipc::MessageType::SendPlayerToLocation, 34, "SendPlayerToLocation" },
        { ipc::MessageType::AssistChannelEvent, 35, "AssistChannelEvent" },
        { ipc::MessageType::GMCallRequest, 36, "GMCallRequest" },
        { ipc::MessageType::GMCallResponse, 37, "GMCallResponse" },
    };
}

auto testGeneratedMessageTypes() -> bool
{
    bool ok = true;
    for (const auto& testCase : messageTypeCases())
    {
        const auto label = std::string(testCase.name);
        ok = expectEqualInt(static_cast<int>(static_cast<uint8_t>(testCase.type)), testCase.code, label + " code") && ok;
        ok = expectEqualString(ipc::toString(testCase.type), testCase.name, label + " string") && ok;
    }

    ok = expectEqualString(ipc::toString(static_cast<ipc::MessageType>(0)), "Unknown", "unknown zero string") && ok;

    return ok;
}

auto testGeneratedTemplateMetadata() -> bool
{
    bool ok = true;

    ok = expectEqualInt(
             static_cast<int>(ipc::EnumTypeV<ipc::EmptyStruct>),
             static_cast<int>(ipc::MessageType::EmptyStruct),
             "EnumTypeV EmptyStruct") &&
         ok;
    ok = expectEqualInt(
             static_cast<int>(ipc::EnumTypeV<ipc::ChatMessageParty>),
             static_cast<int>(ipc::MessageType::ChatMessageParty),
             "EnumTypeV ChatMessageParty") &&
         ok;
    ok = expectEqualInt(
             static_cast<int>(ipc::EnumTypeV<ipc::GMCallResponse>),
             static_cast<int>(ipc::MessageType::GMCallResponse),
             "EnumTypeV GMCallResponse") &&
         ok;

    ok = expectEqualString(ipc::toStringV<ipc::EmptyStruct>, "EmptyStruct", "toStringV EmptyStruct") && ok;
    ok = expectEqualString(ipc::toStringV<ipc::ChatMessageParty>, "ChatMessageParty", "toStringV ChatMessageParty") && ok;
    ok = expectEqualString(ipc::toStringV<ipc::GMCallResponse>, "GMCallResponse", "toStringV GMCallResponse") && ok;

    return ok;
}

} // namespace

auto runIPCMessageTypeSelfTests() -> bool
{
    return testGeneratedMessageTypes() && testGeneratedTemplateMetadata();
}
