#include "test_world_assist_channel_event_1288.h"
#include "world/assist_channel_event.h"
#include "world/char_id_reroute.h"
#include "common/ipp.h"
#include "common/types/maybe.h"
#include <iostream>
#include <limits>
namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition) std::cerr << "world AssistChannelEvent 1288 self-test failed: " << label << '\n';
    return condition;
}
auto testResolvedReceiver() -> bool
{
    const ipc::AssistChannelEvent message{ .senderId = 123, .receiverId = std::numeric_limits<uint32>::max(), .action = 0xFF };
    const IPP endpoint{};
    int lookups{};
    int sends{};
    uint32 lookedUpId{};
    IPP sentEndpoint{};
    ipc::AssistChannelEvent sent{};
    worldipc::HandleAssistChannelEvent(message, [&](uint32 receiverId, const ipc::AssistChannelEvent& delivered)
    {
        worldipc::RerouteMessageToCharId(receiverId, delivered,
            [&](uint32 id) -> Maybe<IPP> { ++lookups; lookedUpId = id; return endpoint; },
            [&](const IPP& target, const ipc::AssistChannelEvent& event) { ++sends; sentEndpoint = target; sent = event; });
    });
    return expect(lookups == 1 && sends == 1, "resolved receiver routed once") &&
           expect(lookedUpId == message.receiverId, "receiver ID selected instead of sender ID") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "all-zero endpoint receives event") &&
           expect(sent.senderId == message.senderId && sent.receiverId == message.receiverId && sent.action == message.action, "event forwarded unchanged");
}
auto testMissingReceiver() -> bool
{
    int lookups{};
    int sends{};
    worldipc::HandleAssistChannelEvent(ipc::AssistChannelEvent{}, [&](uint32 receiverId, const ipc::AssistChannelEvent& delivered)
    {
        worldipc::RerouteMessageToCharId(receiverId, delivered,
            [&](uint32 id) -> Maybe<IPP> { ++lookups; return id == 0 ? std::nullopt : Maybe<IPP>{ IPP{} }; },
            [&](const IPP&, const ipc::AssistChannelEvent&) { ++sends; });
    });
    return expect(lookups == 1, "zero receiver looked up once") && expect(sends == 0, "missing receiver sends nothing");
}
} // namespace
auto runWorldAssistChannelEvent1288SelfTests() -> bool { return testResolvedReceiver() && testMissingReceiver(); }
