#include "test_map_standard_message_delivery_1293.h"

#include "map/standard_message_delivery.h"

#include <iostream>
#include <string>

namespace
{

struct FakePlayer
{
};

struct Harness
{
    FakePlayer  player{};
    FakePlayer* lookupResult{ &player };
    uint32      lookedUpId{};
    int         lookupCalls{};
    int         stringCalls{};
    int         parameterCalls{};
    FakePlayer* deliveredPlayer{};
    std::string string2{};
    uint32      param0{};
    uint32      param1{};
    MsgStd      message{};

    auto lookup(const uint32 id) -> FakePlayer*
    {
        ++lookupCalls;
        lookedUpId = id;
        return lookupResult;
    }

    void deliverString(FakePlayer* target, const std::string& value, const MsgStd messageId)
    {
        ++stringCalls;
        deliveredPlayer = target;
        string2         = value;
        message         = messageId;
    }

    void deliverParameters(FakePlayer* target, const uint32 first, const uint32 second, const MsgStd messageId)
    {
        ++parameterCalls;
        deliveredPlayer = target;
        param0          = first;
        param1          = second;
        message         = messageId;
    }
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map standard message delivery 1293 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testStandardStringDelivery() -> bool
{
    Harness harness{};
    mapipc::HandleMessageStandard(
        ipc::MessageStandard{
            .recipientId = 0xFFFFFFFF,
            .message     = MsgStd::GivenThumbsUp,
            .string2     = "mentor",
        },
        [&](const uint32 id)
        {
            return harness.lookup(id);
        },
        [&](FakePlayer* player, const std::string& string2, const MsgStd messageId)
        {
            harness.deliverString(player, string2, messageId);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            harness.deliverParameters(player, param0, param1, messageId);
        });

    return expect(harness.lookupCalls == 1 && harness.lookedUpId == 0xFFFFFFFF, "string recipient lookup") &&
           expect(harness.stringCalls == 1 && harness.parameterCalls == 0, "string delivery selected") &&
           expect(harness.deliveredPlayer == &harness.player && harness.string2 == "mentor", "string target and value") &&
           expect(harness.message == MsgStd::GivenThumbsUp, "string message ID");
}

auto testStandardParameterDelivery() -> bool
{
    Harness harness{};
    mapipc::HandleMessageStandard(
        ipc::MessageStandard{
            .recipientId = 0xABCDEF01,
            .message     = MsgStd::TargetIsCurrentlyBlocking,
            .param0      = 0xFFFFFFFE,
            .param1      = 0x87654321,
            .string2     = "ignored",
        },
        [&](const uint32 id)
        {
            return harness.lookup(id);
        },
        [&](FakePlayer* player, const std::string& string2, const MsgStd messageId)
        {
            harness.deliverString(player, string2, messageId);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            harness.deliverParameters(player, param0, param1, messageId);
        });

    return expect(harness.lookupCalls == 1 && harness.lookedUpId == 0xABCDEF01, "parameter recipient lookup") &&
           expect(harness.stringCalls == 0 && harness.parameterCalls == 1, "parameter delivery selected") &&
           expect(harness.deliveredPlayer == &harness.player, "parameter target") &&
           expect(harness.param0 == 0xFFFFFFFE && harness.param1 == 0x87654321, "parameters preserved") &&
           expect(harness.message == MsgStd::TargetIsCurrentlyBlocking, "parameter message ID");
}

auto testEmptyStringUsesParameters() -> bool
{
    Harness harness{};
    mapipc::HandleMessageStandard(
        ipc::MessageStandard{ .message = MsgStd::BlockedByBlockaid },
        [&](const uint32 id)
        {
            return harness.lookup(id);
        },
        [&](FakePlayer* player, const std::string& string2, const MsgStd messageId)
        {
            harness.deliverString(player, string2, messageId);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            harness.deliverParameters(player, param0, param1, messageId);
        });

    return expect(harness.stringCalls == 0 && harness.parameterCalls == 1, "empty string parameter delivery") &&
           expect(harness.param0 == 0 && harness.param1 == 0 && harness.message == MsgStd::BlockedByBlockaid, "zero parameters preserved");
}

auto testSystemAlwaysUsesParameters() -> bool
{
    Harness harness{};
    mapipc::HandleMessageSystem(
        ipc::MessageSystem{
            .recipientId = 0x10203040,
            .message     = MsgStd::GivenWarning,
            .param0      = 0xA0B0C0D0,
            .param1      = 0xE0F00102,
        },
        [&](const uint32 id)
        {
            return harness.lookup(id);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            harness.deliverParameters(player, param0, param1, messageId);
        });

    return expect(harness.lookupCalls == 1 && harness.lookedUpId == 0x10203040, "system recipient lookup") &&
           expect(harness.parameterCalls == 1 && harness.deliveredPlayer == &harness.player, "system parameter delivery") &&
           expect(harness.param0 == 0xA0B0C0D0 && harness.param1 == 0xE0F00102, "system parameters") &&
           expect(harness.message == MsgStd::GivenWarning, "system message ID");
}

auto testMissingRecipientsDoNothing() -> bool
{
    Harness standard{};
    standard.lookupResult = nullptr;
    mapipc::HandleMessageStandard(
        ipc::MessageStandard{ .recipientId = 77, .string2 = "mentor" },
        [&](const uint32 id)
        {
            return standard.lookup(id);
        },
        [&](FakePlayer* player, const std::string& string2, const MsgStd messageId)
        {
            standard.deliverString(player, string2, messageId);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            standard.deliverParameters(player, param0, param1, messageId);
        });

    Harness system{};
    system.lookupResult = nullptr;
    mapipc::HandleMessageSystem(
        ipc::MessageSystem{ .recipientId = 88 },
        [&](const uint32 id)
        {
            return system.lookup(id);
        },
        [&](FakePlayer* player, const uint32 param0, const uint32 param1, const MsgStd messageId)
        {
            system.deliverParameters(player, param0, param1, messageId);
        });

    return expect(standard.lookupCalls == 1 && standard.lookedUpId == 77, "missing standard lookup") &&
           expect(standard.stringCalls == 0 && standard.parameterCalls == 0, "missing standard no delivery") &&
           expect(system.lookupCalls == 1 && system.lookedUpId == 88, "missing system lookup") &&
           expect(system.parameterCalls == 0, "missing system no delivery");
}

} // namespace

auto runMapStandardMessageDelivery1293SelfTests() -> bool
{
    return testStandardStringDelivery() && testStandardParameterDelivery() && testEmptyStringUsesParameters() &&
           testSystemAlwaysUsesParameters() && testMissingRecipientsDoNothing();
}
