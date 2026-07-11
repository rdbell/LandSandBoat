#include "test_world_entity_information_request_1285.h"

#include "world/entity_information_request.h"

#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world EntityInformationRequest 1285 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testPlayerRoute() -> bool
{
    const ipc::EntityInformationRequest message{
        .requesterId = 11,
        .targetId = std::numeric_limits<uint32>::max(),
        .entityType = 0x01,
        .warp = true,
        .spawnedOnly = true,
    };
    int charCalls{};
    int zoneCalls{};
    uint32 charId{};
    ipc::EntityInformationRequest delivered{};
    worldipc::HandleEntityInformationRequest(
        message,
        [&](uint32 id, const ipc::EntityInformationRequest& request)
        {
            ++charCalls;
            charId = id;
            delivered = request;
        },
        [&](uint16, const ipc::EntityInformationRequest&) { ++zoneCalls; });

    return expect(charCalls == 1 && zoneCalls == 0, "PC type selects only character route") &&
           expect(charId == message.targetId, "full target ID used as character key") &&
           expect(delivered.requesterId == message.requesterId && delivered.warp && delivered.spawnedOnly, "message forwarded unchanged");
}

auto testZoneRoutes() -> bool
{
    for (const auto entityType : { uint8{ 0x00 }, uint8{ 0x02 }, uint8{ 0x03 }, uint8{ 0x04 }, uint8{ 0xFF } })
    {
        const ipc::EntityInformationRequest message{
            .requesterId = 22,
            .targetId = 0xFABCD678,
            .entityType = entityType,
        };
        int charCalls{};
        int zoneCalls{};
        uint16 zoneId{};
        ipc::EntityInformationRequest delivered{};
        worldipc::HandleEntityInformationRequest(
            message,
            [&](uint32, const ipc::EntityInformationRequest&) { ++charCalls; },
            [&](uint16 id, const ipc::EntityInformationRequest& request)
            {
                ++zoneCalls;
                zoneId = id;
                delivered = request;
            });

        if (!expect(charCalls == 0 && zoneCalls == 1, "non-PC type selects only zone route") ||
            !expect(zoneId == 0x0BCD, "zone route masks shifted target ID to 12 bits") ||
            !expect(delivered.targetId == message.targetId && delivered.entityType == entityType, "zone message forwarded unchanged"))
        {
            return false;
        }
    }
    return true;
}

auto testZeroZoneRoute() -> bool
{
    const ipc::EntityInformationRequest message{
        .requesterId = 33,
        .targetId = 0,
        .entityType = 0x02,
        .warp = true,
    };
    int charCalls{};
    int zoneCalls{};
    uint16 zoneId{ 1 };
    ipc::EntityInformationRequest delivered{};
    worldipc::HandleEntityInformationRequest(
        message,
        [&](uint32, const ipc::EntityInformationRequest&) { ++charCalls; },
        [&](uint16 id, const ipc::EntityInformationRequest& request)
        {
            ++zoneCalls;
            zoneId = id;
            delivered = request;
        });

    return expect(charCalls == 0 && zoneCalls == 1, "zero zone still selects zone route once") &&
           expect(zoneId == 0, "zero target derives zone zero") &&
           expect(delivered.requesterId == message.requesterId && delivered.warp, "zero-zone message forwarded unchanged");
}

} // namespace

auto runWorldEntityInformationRequest1285SelfTests() -> bool
{
    return testPlayerRoute() && testZoneRoutes() && testZeroZoneRoute();
}
