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

#include "test_ipc_chat_payloads.h"

#include "common/ipc_structs.h"
#include "map/enums/chat_message_type.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace
{

struct ChatTypeCase
{
    CHAT_MESSAGE_TYPE type;
    uint8_t           code;
    std::string_view  name;
};

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC chat payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC chat payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto expectEqualBool(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC chat payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto chatTypeCases() -> std::vector<ChatTypeCase>
{
    return {
        { MESSAGE_SAY, 0, "MESSAGE_SAY" },
        { MESSAGE_SHOUT, 1, "MESSAGE_SHOUT" },
        { MESSAGE_UNKNOWN, 2, "MESSAGE_UNKNOWN" },
        { MESSAGE_TELL, 3, "MESSAGE_TELL" },
        { MESSAGE_PARTY, 4, "MESSAGE_PARTY" },
        { MESSAGE_LINKSHELL, 5, "MESSAGE_LINKSHELL" },
        { MESSAGE_SYSTEM_1, 6, "MESSAGE_SYSTEM_1" },
        { MESSAGE_SYSTEM_2, 7, "MESSAGE_SYSTEM_2" },
        { MESSAGE_EMOTION, 8, "MESSAGE_EMOTION" },
        { MESSAGE_GMPROMPT, 12, "MESSAGE_GMPROMPT" },
        { MESSAGE_NS_SAY, 13, "MESSAGE_NS_SAY" },
        { MESSAGE_NS_SHOUT, 14, "MESSAGE_NS_SHOUT" },
        { MESSAGE_NS_PARTY, 15, "MESSAGE_NS_PARTY" },
        { MESSAGE_NS_LINKSHELL, 16, "MESSAGE_NS_LINKSHELL" },
        { MESSAGE_UNKNOWN_17, 17, "MESSAGE_UNKNOWN_17" },
        { MESSAGE_UNKNOWN_18, 18, "MESSAGE_UNKNOWN_18" },
        { MESSAGE_UNKNOWN_19, 19, "MESSAGE_UNKNOWN_19" },
        { MESSAGE_UNKNOWN_20, 20, "MESSAGE_UNKNOWN_20" },
        { MESSAGE_UNKNOWN_21, 21, "MESSAGE_UNKNOWN_21" },
        { MESSAGE_UNKNOWN_22, 22, "MESSAGE_UNKNOWN_22" },
        { MESSAGE_UNKNOWN_23, 23, "MESSAGE_UNKNOWN_23" },
        { MESSAGE_UNKNOWN_24, 24, "MESSAGE_UNKNOWN_24" },
        { MESSAGE_UNKNOWN_25, 25, "MESSAGE_UNKNOWN_25" },
        { MESSAGE_YELL, 26, "MESSAGE_YELL" },
        { MESSAGE_LINKSHELL2, 27, "MESSAGE_LINKSHELL2" },
        { MESSAGE_NS_LINKSHELL2, 28, "MESSAGE_NS_LINKSHELL2" },
        { MESSAGE_SYSTEM_3, 29, "MESSAGE_SYSTEM_3" },
        { MESSAGE_LINKSHELL3, 30, "MESSAGE_LINKSHELL3" },
        { MESSAGE_NS_LINKSHELL3, 31, "MESSAGE_NS_LINKSHELL3" },
        { MESSAGE_UNKNOWN_32, 32, "MESSAGE_UNKNOWN_32" },
        { MESSAGE_UNITY, 33, "MESSAGE_UNITY" },
        { MESSAGE_JP_ASSIST, 34, "MESSAGE_JP_ASSIST" },
        { MESSAGE_NA_ASSIST, 35, "MESSAGE_NA_ASSIST" },
    };
}

auto testChatMessageTypes() -> bool
{
    bool ok = true;
    for (const auto& testCase : chatTypeCases())
    {
        ok = expectEqualInt(static_cast<int>(testCase.type), testCase.code, std::string(testCase.name)) && ok;
    }
    return ok;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const ipc::ChatMessageTell tell{};
    ok = expectEqualInt(tell.senderId, 0, "tell sender id") && ok;
    ok = expectEqualString(tell.senderName, "", "tell sender name") && ok;
    ok = expectEqualString(tell.recipientName, "", "tell recipient name") && ok;
    ok = expectEqualString(tell.message, "", "tell message") && ok;
    ok = expectEqualInt(tell.zoneId, 0, "tell zone") && ok;
    ok = expectEqualInt(tell.gmLevel, 0, "tell gm level") && ok;

    ok = expectEqualInt(ipc::ChatMessageParty{}.messageType, MESSAGE_PARTY, "party message type") && ok;
    ok = expectEqualInt(ipc::ChatMessageAlliance{}.messageType, MESSAGE_PARTY, "alliance message type") && ok;
    ok = expectEqualInt(ipc::ChatMessageUnity{}.messageType, MESSAGE_UNITY, "unity message type") && ok;
    ok = expectEqualInt(ipc::ChatMessageYell{}.messageType, MESSAGE_YELL, "yell message type") && ok;

    const ipc::ChatMessageAssist assist{};
    ok = expectEqualInt(assist.mentorRank, 0, "assist mentor rank") && ok;
    ok = expectEqualInt(assist.masteryRank, 1, "assist mastery rank") && ok;
    ok = expectEqualInt(assist.gmLevel, 0, "assist gm level") && ok;
    ok = expectEqualInt(assist.messageType, MESSAGE_NA_ASSIST, "assist message type") && ok;

    const ipc::ChatMessageServerMessage serverMessage{};
    ok = expectEqualInt(serverMessage.messageType, MESSAGE_SYSTEM_1, "server message type") && ok;
    ok = expectEqualBool(serverMessage.skipSender, false, "server skip sender") && ok;
    ok = expectEqualInt(ipc::ChatMessageCustom{}.messageType, MESSAGE_SAY, "custom message type") && ok;

    return ok;
}

} // namespace

auto runIPCChatPayloadSelfTests() -> bool
{
    return testChatMessageTypes() && testDefaultPayloads();
}
