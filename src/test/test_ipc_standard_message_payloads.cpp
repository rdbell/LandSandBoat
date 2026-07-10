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

#include "test_ipc_standard_message_payloads.h"

#include "common/ipc_structs.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

struct MsgStdValue
{
    MsgStd      message;
    int         expected;
    std::string label;
};

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC standard message payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC standard message payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testMsgStdValues() -> bool
{
    const std::vector<MsgStdValue> tests{
        { MsgStd::CouldNotEnter, 2, "CouldNotEnter" },
        { MsgStd::CouldNotEnterYourRoom, 5, "CouldNotEnterYourRoom" },
        { MsgStd::CallPlacedInTheQueue, 20, "CallPlacedInTheQueue" },
        { MsgStd::TellNotReceivedOffline, 125, "TellNotReceivedOffline" },
        { MsgStd::MoogleUsesItemOnPLant, 136, "MoogleUsesItemOnPLant" },
        { MsgStd::ChocoboRefusedToEnte, 138, "ChocoboRefusedToEnte" },
        { MsgStd::CannotPerformInConflict, 216, "CannotPerformInConflict" },
        { MsgStd::CannotPerformPreparingBattle, 216, "CannotPerformPreparingBattle" },
        { MsgStd::TargetIsCurrentlyBlocking, 225, "TargetIsCurrentlyBlocking" },
        { MsgStd::BlockedByBlockaid, 226, "BlockedByBlockaid" },
        { MsgStd::GivenThumbsUp, 306, "GivenThumbsUp" },
        { MsgStd::GivenWarning, 307, "GivenWarning" },
        { MsgStd::AnErrorHasOccured, 308, "AnErrorHasOccured" },
        { MsgStd::AssistChannelExpired, 318, "AssistChannelExpired" },
        { MsgStd::LevelSyncActivated, 540, "LevelSyncActivated" },
        { MsgStd::TreasureHunterProc, 603, "TreasureHunterProc" },
    };

    bool ok = true;
    for (const auto& test : tests)
    {
        ok = expectEqualInt(static_cast<int>(test.message), test.expected, test.label) && ok;
    }
    return ok;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const ipc::MessageStandard standard{};
    ok = expectEqualInt(standard.recipientId, 0, "standard recipient id default") && ok;
    ok = expectEqualInt(static_cast<int>(standard.message), 0, "standard message default") && ok;
    ok = expectEqualInt(standard.param0, 0, "standard param0 default") && ok;
    ok = expectEqualInt(standard.param1, 0, "standard param1 default") && ok;
    ok = expectEqualString(standard.string2, "", "standard string2 default") && ok;

    const ipc::MessageSystem system{};
    ok = expectEqualInt(system.recipientId, 0, "system recipient id default") && ok;
    ok = expectEqualInt(static_cast<int>(system.message), 0, "system message default") && ok;
    ok = expectEqualInt(system.param0, 0, "system param0 default") && ok;
    ok = expectEqualInt(system.param1, 0, "system param1 default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    const ipc::MessageStandard standard{
        .recipientId = 1001,
        .message     = MsgStd::GivenThumbsUp,
        .param0      = 2002,
        .param1      = 3003,
        .string2     = "mentor",
    };
    ok = expectEqualInt(standard.recipientId, 1001, "assigned standard recipient id") && ok;
    ok = expectEqualInt(static_cast<int>(standard.message), 306, "assigned standard message") && ok;
    ok = expectEqualInt(standard.param0, 2002, "assigned standard param0") && ok;
    ok = expectEqualInt(standard.param1, 3003, "assigned standard param1") && ok;
    ok = expectEqualString(standard.string2, "mentor", "assigned standard string2") && ok;

    const ipc::MessageSystem system{
        .recipientId = 4004,
        .message     = MsgStd::TargetIsCurrentlyBlocking,
        .param0      = 5005,
        .param1      = 6006,
    };
    ok = expectEqualInt(system.recipientId, 4004, "assigned system recipient id") && ok;
    ok = expectEqualInt(static_cast<int>(system.message), 225, "assigned system message") && ok;
    ok = expectEqualInt(system.param0, 5005, "assigned system param0") && ok;
    ok = expectEqualInt(system.param1, 6006, "assigned system param1") && ok;

    return ok;
}

auto usesStringPacketForm(const ipc::MessageStandard& message) -> bool
{
    return !message.string2.empty() && message.param0 == 0 && message.param1 == 0;
}

auto testMapIPCStandardMessagePacketSelection() -> bool
{
    bool ok = true;

    ok = expectEqualInt(usesStringPacketForm(ipc::MessageStandard{ .string2 = "mentor" }), true, "string-only packet form") && ok;
    ok = expectEqualInt(usesStringPacketForm(ipc::MessageStandard{}), false, "empty string uses parameter form") && ok;
    ok = expectEqualInt(usesStringPacketForm(ipc::MessageStandard{ .param0 = 1, .string2 = "mentor" }), false, "param0 overrides string") && ok;
    ok = expectEqualInt(usesStringPacketForm(ipc::MessageStandard{ .param1 = 1, .string2 = "mentor" }), false, "param1 overrides string") && ok;
    ok = expectEqualInt(usesStringPacketForm(ipc::MessageStandard{ .param0 = 1, .param1 = 2, .string2 = "mentor" }), false, "both params override string") && ok;

    return ok;
}

} // namespace

auto runIPCStandardMessagePayloadSelfTests() -> bool
{
    return testMsgStdValues() && testDefaultPayloads() && testAssignedPayloads() && testMapIPCStandardMessagePacketSelection();
}
