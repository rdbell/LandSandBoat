#include "test_world_message_system_1275.h"

#include "world/char_id_reroute.h"
#include "world/message_system.h"

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
        std::cerr << "world MessageSystem 1275 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::MessageSystem& left, const ipc::MessageSystem& right) -> bool
{
    return left.recipientId == right.recipientId && left.message == right.message &&
           left.param0 == right.param0 && left.param1 == right.param1;
}

auto testResolvedRecipient() -> bool
{
    const ipc::MessageSystem message{
        .recipientId = std::numeric_limits<uint32>::max(),
        .message     = MsgStd::TargetIsCurrentlyBlocking,
        .param0      = std::numeric_limits<uint32>::max() - 1,
        .param1      = std::numeric_limits<uint32>::max() - 2,
    };
    const auto endpoint = IPP{};
    int        rerouteCalls{};
    int        lookupCalls{};
    int        sendCalls{};
    uint32     lookedUpId{};
    IPP        sentEndpoint{};
    ipc::MessageSystem sentMessage{};

    worldipc::HandleMessageSystem(
        message,
        [&](const uint32 recipientId, const ipc::MessageSystem& delivered)
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
                [&](const IPP& target, const ipc::MessageSystem& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved recipient routed once") &&
           expect(lookedUpId == message.recipientId, "recipient ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives message") &&
           expect(sameMessage(sentMessage, message), "full system-message payload forwarded unchanged");
}

auto testMissingRecipient() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleMessageSystem(
        ipc::MessageSystem{},
        [&](const uint32 recipientId, const ipc::MessageSystem& delivered)
        {
            worldipc::RerouteMessageToCharId(
                recipientId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::MessageSystem&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero recipient looked up once") &&
           expect(sendCalls == 0, "missing recipient sends nothing");
}

} // namespace

auto runWorldMessageSystem1275SelfTests() -> bool
{
    return testResolvedRecipient() && testMissingRecipient();
}
