#include "test_world_chat_message_custom_1266.h"

#include "world/char_id_reroute.h"
#include "world/chat_message_custom.h"

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageCustom 1266 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageCustom& left, const ipc::ChatMessageCustom& right) -> bool
{
    return left.recipientId == right.recipientId && left.senderName == right.senderName && left.message == right.message &&
           left.messageType == right.messageType;
}

auto testResolvedRecipient() -> bool
{
    const ipc::ChatMessageCustom message{
        .recipientId = std::numeric_limits<uint32>::max(),
        .senderName  = "Sender",
        .message     = "payload",
        .messageType = MESSAGE_SYSTEM_3,
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    uint32     lookedUpId{};
    IPP        sentEndpoint{};
    ipc::ChatMessageCustom sentMessage{};

    worldipc::HandleChatMessageCustom(
        message,
        [&](const uint32 recipientId, const ipc::ChatMessageCustom& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharId(
                recipientId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::ChatMessageCustom& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved recipient routed once") &&
           expect(lookedUpId == message.recipientId, "recipient ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives message") &&
           expect(sameMessage(sentMessage, message), "full custom payload forwarded unchanged");
}

auto testMissingRecipient() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleChatMessageCustom(
        ipc::ChatMessageCustom{ .recipientId = 0, .senderName = "", .message = "", .messageType = MESSAGE_SAY },
        [&](const uint32 recipientId, const ipc::ChatMessageCustom& delivered)
        {
            worldipc::RerouteMessageToCharId(
                recipientId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::ChatMessageCustom&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero recipient looked up once") &&
           expect(sendCalls == 0, "missing recipient sends nothing");
}

} // namespace

auto runWorldChatMessageCustom1266SelfTests() -> bool
{
    return testResolvedRecipient() && testMissingRecipient();
}
