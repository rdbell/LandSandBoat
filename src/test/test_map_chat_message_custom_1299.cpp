#include "test_map_chat_message_custom_1299.h"

#include "map/chat_message_custom.h"

#include <iostream>
#include <limits>
#include <string>
#include <utility>

namespace
{

struct FakeCharacter
{
    bool disappeared{};
    bool inPrison{};
    int  deliveries{};
};

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map chat message custom 1299 self-test failed: " << label << '\n';
    }
    return condition;
}

auto deliver(const ipc::ChatMessageCustom& message, FakeCharacter* character) -> ipc::ChatMessageCustom
{
    ipc::ChatMessageCustom delivered{};
    mapipc::HandleChatMessageCustom(
        message,
        [character](const uint32)
        {
            return character;
        },
        [](FakeCharacter* target)
        {
            return std::pair{ target->disappeared, target->inPrison };
        },
        [&](FakeCharacter* target, const ipc::ChatMessageCustom& chat)
        {
            ++target->deliveries;
            delivered = chat;
            expect(target == character, "custom delivery target");
        });
    return delivered;
}

auto testEligibilityGate() -> bool
{
    return expect(mapipc::CanReceiveCustomChat(false, false), "eligible") &&
           expect(!mapipc::CanReceiveCustomChat(true, false), "disappeared blocks") &&
           expect(!mapipc::CanReceiveCustomChat(false, true), "prison blocks") &&
           expect(!mapipc::CanReceiveCustomChat(true, true), "both block");
}

auto testPresentAndBlocked() -> bool
{
    FakeCharacter eligible{};
    const ipc::ChatMessageCustom message{
        .recipientId = std::numeric_limits<uint32>::max(),
        .senderName  = "CustomSender",
        .message     = "custom body",
        .messageType = MESSAGE_SYSTEM_1,
    };

    const auto delivered = deliver(message, &eligible);

    FakeCharacter disappeared{ .disappeared = true };
    deliver(message, &disappeared);

    FakeCharacter imprisoned{ .inPrison = true };
    deliver(message, &imprisoned);

    return expect(eligible.deliveries == 1, "eligible delivery") &&
           expect(delivered.recipientId == message.recipientId && delivered.senderName == "CustomSender", "sender fields") &&
           expect(delivered.message == "custom body" && delivered.messageType == MESSAGE_SYSTEM_1, "body and type") &&
           expect(disappeared.deliveries == 0 && imprisoned.deliveries == 0, "blocked recipients no-op");
}

auto testMissingRecipient() -> bool
{
    int deliveries{};
    uint32 lookedUpId{};
    mapipc::HandleChatMessageCustom(
        ipc::ChatMessageCustom{ .recipientId = 0x10203040 },
        [&](const uint32 recipientId) -> FakeCharacter*
        {
            lookedUpId = recipientId;
            return nullptr;
        },
        [](FakeCharacter*)
        {
            return std::pair{ false, false };
        },
        [&](FakeCharacter*, const ipc::ChatMessageCustom&)
        {
            ++deliveries;
        });

    return expect(lookedUpId == 0x10203040u, "recipient lookup id") &&
           expect(deliveries == 0, "missing recipient no-op");
}

} // namespace

auto runMapChatMessageCustom1299SelfTests() -> bool
{
    return testEligibilityGate() && testPresentAndBlocked() && testMissingRecipient();
}
