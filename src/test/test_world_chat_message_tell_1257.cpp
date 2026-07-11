#include "test_world_chat_message_tell_1257.h"

#include "world/chat_message_tell.h"

#include "common/types/maybe.h"

#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ChatMessageTell 1257 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testLookupHitForwardsUnchangedTell() -> bool
{
    const auto source   = IPP(0x0100007F, 54000);
    const IPP  resolved{};
    const ipc::ChatMessageTell message{
        .senderId      = std::numeric_limits<uint32>::max(),
        .senderName    = "Sender",
        .recipientName = "",
        .message       = "payload",
        .zoneId        = std::numeric_limits<uint16>::max(),
        .gmLevel       = std::numeric_limits<uint8>::max(),
    };
    int                  lookupCalls{};
    int                  forwardCalls{};
    int                  offlineCalls{};
    std::string          lookedUpName{ "sentinel" };
    IPP                  forwardedEndpoint{};
    ipc::ChatMessageTell forwardedMessage{};
    std::vector<char>    order{};

    worldipc::HandleChatMessageTell(
        source,
        message,
        [&](const std::string& recipientName) -> Maybe<IPP>
        {
            order.push_back('L');
            ++lookupCalls;
            lookedUpName = recipientName;
            return resolved;
        },
        [&](const IPP& endpoint, const ipc::ChatMessageTell& delivered)
        {
            order.push_back('F');
            ++forwardCalls;
            forwardedEndpoint = endpoint;
            forwardedMessage  = delivered;
        },
        [&](const IPP&, const ipc::MessageStandard&)
        {
            ++offlineCalls;
        });

    return expect(lookupCalls == 1 && lookedUpName.empty(), "empty recipient name looked up exactly once") &&
           expect(forwardCalls == 1 && offlineCalls == 0, "hit forwards only") &&
           expect(forwardedEndpoint.getRawIPP() == resolved.getRawIPP(), "hit accepts resolved zero endpoint") &&
           expect(forwardedMessage.senderId == message.senderId && forwardedMessage.senderName == message.senderName &&
                      forwardedMessage.recipientName == message.recipientName && forwardedMessage.message == message.message &&
                      forwardedMessage.zoneId == message.zoneId && forwardedMessage.gmLevel == message.gmLevel,
                  "hit forwards unchanged tell") &&
           expect(order == std::vector<char>{ 'L', 'F' }, "hit operation order");
}

auto testLookupMissRepliesOfflineToSource() -> bool
{
    const auto source = IPP(0x0300007F, 54002);
    const ipc::ChatMessageTell message{
        .senderId      = 0,
        .senderName    = "",
        .recipientName = "Missing",
        .message       = "",
        .zoneId        = 0,
        .gmLevel       = 0,
    };
    int                  lookupCalls{};
    int                  forwardCalls{};
    int                  offlineCalls{};
    IPP                  offlineEndpoint{ 1, 1 };
    ipc::MessageStandard offlineMessage{};
    std::vector<char>    order{};

    worldipc::HandleChatMessageTell(
        source,
        message,
        [&](const std::string& recipientName) -> Maybe<IPP>
        {
            order.push_back('L');
            ++lookupCalls;
            return recipientName == message.recipientName ? std::nullopt : Maybe<IPP>{ IPP{} };
        },
        [&](const IPP&, const ipc::ChatMessageTell&)
        {
            ++forwardCalls;
        },
        [&](const IPP& endpoint, const ipc::MessageStandard& delivered)
        {
            order.push_back('O');
            ++offlineCalls;
            offlineEndpoint = endpoint;
            offlineMessage  = delivered;
        });

    return expect(lookupCalls == 1 && forwardCalls == 0 && offlineCalls == 1, "miss replies offline only") &&
           expect(offlineEndpoint.getRawIPP() == source.getRawIPP(), "miss replies to exact nonzero source endpoint") &&
           expect(offlineMessage.recipientId == 0 && offlineMessage.message == MsgStd::TellNotReceivedOffline,
                  "offline reply targets exact zero sender and message code") &&
           expect(offlineMessage.param0 == 0 && offlineMessage.param1 == 0 && offlineMessage.string2.empty(),
                  "offline reply leaves trailing fields defaulted") &&
           expect(order == std::vector<char>{ 'L', 'O' }, "miss operation order");
}

} // namespace

auto runWorldChatMessageTell1257SelfTests() -> bool
{
    return testLookupHitForwardsUnchangedTell() && testLookupMissRepliesOfflineToSource();
}
