#include "test_world_message_standard_1274.h"

#include "world/char_id_reroute.h"
#include "world/message_standard.h"

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
        std::cerr << "world MessageStandard 1274 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::MessageStandard& left, const ipc::MessageStandard& right) -> bool
{
    return left.recipientId == right.recipientId && left.message == right.message &&
           left.param0 == right.param0 && left.param1 == right.param1 && left.string2 == right.string2;
}

auto testResolvedRecipient() -> bool
{
    const ipc::MessageStandard message{
        .recipientId = std::numeric_limits<uint32>::max(),
        .message     = MsgStd::GivenThumbsUp,
        .param0      = std::numeric_limits<uint32>::max() - 1,
        .param1      = std::numeric_limits<uint32>::max() - 2,
        .string2     = "mentor",
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    uint32     lookedUpId{};
    IPP        sentEndpoint{};
    ipc::MessageStandard sentMessage{};

    worldipc::HandleMessageStandard(
        message,
        [&](const uint32 recipientId, const ipc::MessageStandard& delivered)
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
                [&](const IPP& target, const ipc::MessageStandard& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved recipient routed once") &&
           expect(lookedUpId == message.recipientId, "recipient ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives message") &&
           expect(sameMessage(sentMessage, message), "full standard-message payload forwarded unchanged");
}

auto testMissingRecipient() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleMessageStandard(
        ipc::MessageStandard{},
        [&](const uint32 recipientId, const ipc::MessageStandard& delivered)
        {
            worldipc::RerouteMessageToCharId(
                recipientId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::MessageStandard&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero recipient looked up once") &&
           expect(sendCalls == 0, "missing recipient sends nothing");
}

} // namespace

auto runWorldMessageStandard1274SelfTests() -> bool
{
    return testResolvedRecipient() && testMissingRecipient();
}
