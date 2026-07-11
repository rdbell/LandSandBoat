#include "test_world_entity_information_response_1286.h"

#include "world/char_id_reroute.h"
#include "world/entity_information_response.h"

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
        std::cerr << "world EntityInformationResponse 1286 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testResolvedRequester() -> bool
{
    const ipc::EntityInformationResponse message{
        .requesterId = std::numeric_limits<uint32>::max(),
        .targetId = 7007,
        .entityType = 0xFF,
        .warp = true,
        .zoneId = std::numeric_limits<uint16>::max(),
        .x = 1.5F,
        .y = -2.25F,
        .z = 3.75F,
        .rot = 0xFF,
        .moghouseId = 9009,
    };
    const IPP endpoint{};
    int lookupCalls{};
    int sendCalls{};
    uint32 lookedUpId{};
    IPP sentEndpoint{};
    ipc::EntityInformationResponse sent{};
    worldipc::HandleEntityInformationResponse(
        message,
        [&](uint32 requesterId, const ipc::EntityInformationResponse& delivered)
        {
            worldipc::RerouteMessageToCharId(
                requesterId,
                delivered,
                [&](uint32 id) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpId = id;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::EntityInformationResponse& response)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sent = response;
                });
        });

    return expect(lookupCalls == 1 && sendCalls == 1, "resolved requester routed once") &&
           expect(lookedUpId == message.requesterId, "requester ID selected instead of target ID") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "all-zero endpoint receives response") &&
           expect(sent.requesterId == message.requesterId && sent.targetId == message.targetId && sent.entityType == message.entityType &&
                      sent.warp == message.warp && sent.zoneId == message.zoneId && sent.x == message.x && sent.y == message.y && sent.z == message.z &&
                      sent.rot == message.rot && sent.moghouseId == message.moghouseId,
                  "response forwarded unchanged");
}

auto testMissingRequester() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandleEntityInformationResponse(
        ipc::EntityInformationResponse{},
        [&](uint32 requesterId, const ipc::EntityInformationResponse& delivered)
        {
            worldipc::RerouteMessageToCharId(
                requesterId,
                delivered,
                [&](uint32 id) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return id == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::EntityInformationResponse&) { ++sendCalls; });
        });
    return expect(lookupCalls == 1, "zero requester looked up once") &&
           expect(sendCalls == 0, "missing requester sends nothing");
}

} // namespace

auto runWorldEntityInformationResponse1286SelfTests() -> bool
{
    return testResolvedRequester() && testMissingRequester();
}
