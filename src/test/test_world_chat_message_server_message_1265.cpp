#include "test_world_chat_message_server_message_1265.h"

#include "world/chat_message_server_message.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageServerMessage 1265 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageServerMessage& left, const ipc::ChatMessageServerMessage& right) -> bool
{
    return left.senderId == right.senderId && left.senderName == right.senderName && left.message == right.message &&
           left.zoneId == right.zoneId && left.gmLevel == right.gmLevel && left.messageType == right.messageType &&
           left.skipSender == right.skipSender;
}

auto testFanout() -> bool
{
    const ipc::ChatMessageServerMessage message{
        .senderId    = std::numeric_limits<uint32>::max(),
        .senderName  = "Sender",
        .message     = "payload",
        .zoneId      = std::numeric_limits<uint16>::max(),
        .gmLevel     = std::numeric_limits<uint8>::max(),
        .messageType = MESSAGE_SYSTEM_3,
        .skipSender  = true,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x14131211, 45678), IPP{} };
    int                    lookupCalls{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::ChatMessageServerMessage> sentMessages{};
    worldipc::HandleChatMessageServerMessage(
        message,
        [&]
        {
            ++lookupCalls;
            return endpoints;
        },
        [&](const IPP& endpoint, const ipc::ChatMessageServerMessage& delivered)
        {
            sentEndpoints.push_back(endpoint);
            sentMessages.push_back(delivered);
        });

    bool exact = lookupCalls == 1 && sentEndpoints.size() == endpoints.size() && sentMessages.size() == endpoints.size();
    for (std::size_t index = 0; exact && index < endpoints.size(); ++index)
    {
        exact = sentEndpoints[index].getRawIPP() == endpoints[index].getRawIPP() && sameMessage(sentMessages[index], message);
    }

    int emptyLookups{};
    int emptySends{};
    worldipc::HandleChatMessageServerMessage(
        ipc::ChatMessageServerMessage{},
        [&]
        {
            ++emptyLookups;
            return std::vector<IPP>{};
        },
        [&](const IPP&, const ipc::ChatMessageServerMessage&) { ++emptySends; });
    return expect(exact, "one lookup and ordered unchanged fanout") &&
           expect(emptyLookups == 1 && emptySends == 0, "empty catalog still looked up once");
}

} // namespace

auto runWorldChatMessageServerMessage1265SelfTests() -> bool
{
    return testFanout();
}
