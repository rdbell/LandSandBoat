#include "test_world_send_player_to_location_1287.h"

#include "world/char_id_reroute.h"
#include "world/send_player_to_location.h"

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <iostream>
#include <limits>

namespace
{
auto expect(bool condition, const char* label) -> bool
{
    if (!condition) std::cerr << "world SendPlayerToLocation 1287 self-test failed: " << label << '\n';
    return condition;
}

auto testResolvedTarget() -> bool
{
    const ipc::SendPlayerToLocation message{
        .targetId = std::numeric_limits<uint32>::max(),
        .zoneId = std::numeric_limits<uint16>::max(),
        .x = -10.5F,
        .y = 0.25F,
        .z = 99.75F,
        .rot = 0xFF,
        .moghouseId = std::numeric_limits<uint32>::max() - 1,
    };
    const IPP endpoint{};
    int lookups{};
    int sends{};
    uint32 lookedUpId{};
    IPP sentEndpoint{};
    ipc::SendPlayerToLocation sent{};
    worldipc::HandleSendPlayerToLocation(
        message,
        [&](uint32 targetId, const ipc::SendPlayerToLocation& delivered)
        {
            worldipc::RerouteMessageToCharId(
                targetId,
                delivered,
                [&](uint32 id) -> Maybe<IPP> { ++lookups; lookedUpId = id; return endpoint; },
                [&](const IPP& target, const ipc::SendPlayerToLocation& location)
                {
                    ++sends;
                    sentEndpoint = target;
                    sent = location;
                });
        });
    return expect(lookups == 1 && sends == 1, "resolved target routed once") &&
           expect(lookedUpId == message.targetId, "target ID selected") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "all-zero endpoint receives location") &&
           expect(sent.targetId == message.targetId && sent.zoneId == message.zoneId && sent.x == message.x && sent.y == message.y &&
                      sent.z == message.z && sent.rot == message.rot && sent.moghouseId == message.moghouseId,
                  "location forwarded unchanged");
}

auto testMissingTarget() -> bool
{
    int lookups{};
    int sends{};
    worldipc::HandleSendPlayerToLocation(
        ipc::SendPlayerToLocation{},
        [&](uint32 targetId, const ipc::SendPlayerToLocation& delivered)
        {
            worldipc::RerouteMessageToCharId(
                targetId,
                delivered,
                [&](uint32 id) -> Maybe<IPP> { ++lookups; return id == 0 ? std::nullopt : Maybe<IPP>{ IPP{} }; },
                [&](const IPP&, const ipc::SendPlayerToLocation&) { ++sends; });
        });
    return expect(lookups == 1, "zero target looked up once") && expect(sends == 0, "missing target sends nothing");
}
} // namespace

auto runWorldSendPlayerToLocation1287SelfTests() -> bool
{
    return testResolvedTarget() && testMissingTarget();
}
