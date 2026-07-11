#include "test_world_chat_message_assist_1264.h"

#include "world/chat_message_assist.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageAssist 1264 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::ChatMessageAssist& left, const ipc::ChatMessageAssist& right) -> bool
{
    return left.senderId == right.senderId && left.senderName == right.senderName && left.message == right.message &&
           left.mentorRank == right.mentorRank && left.masteryRank == right.masteryRank && left.gmLevel == right.gmLevel &&
           left.messageType == right.messageType;
}

auto testFanout() -> bool
{
    const ipc::ChatMessageAssist message{
        .senderId    = std::numeric_limits<uint32>::max(),
        .senderName  = "Sender",
        .message     = "payload",
        .mentorRank  = std::numeric_limits<uint8>::max(),
        .masteryRank = std::numeric_limits<uint8>::max(),
        .gmLevel     = std::numeric_limits<uint8>::max(),
        .messageType = MESSAGE_SYSTEM_3,
    };
    const std::vector<IPP> endpoints{ IPP{}, IPP(0x14131211, 45678), IPP{} };
    int                    lookupCalls{};
    std::vector<IPP>       sentEndpoints{};
    std::vector<ipc::ChatMessageAssist> sentMessages{};
    worldipc::HandleChatMessageAssist(
        message,
        [&]
        {
            ++lookupCalls;
            return endpoints;
        },
        [&](const IPP& endpoint, const ipc::ChatMessageAssist& delivered)
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
    worldipc::HandleChatMessageAssist(
        ipc::ChatMessageAssist{},
        [&]
        {
            ++emptyLookups;
            return std::vector<IPP>{};
        },
        [&](const IPP&, const ipc::ChatMessageAssist&) { ++emptySends; });
    return expect(exact, "one lookup and ordered unchanged fanout") &&
           expect(emptyLookups == 1 && emptySends == 0, "empty catalog still looked up once");
}

} // namespace

auto runWorldChatMessageAssist1264SelfTests() -> bool
{
    return testFanout();
}
