#include "test_world_lua_function_1279.h"

#include "world/lua_function.h"
#include "world/zone_id_reroute.h"

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
        std::cerr << "world LuaFunction 1279 self-test failed: " << label << '\n';
    }
    return condition;
}

auto sameMessage(const ipc::LuaFunction& left, const ipc::LuaFunction& right) -> bool
{
    return left.requesterZoneId == right.requesterZoneId && left.executorZoneId == right.executorZoneId &&
           left.funcString == right.funcString;
}

auto testResolvedExecutorZone() -> bool
{
    const ipc::LuaFunction message{
        .requesterZoneId = std::numeric_limits<uint16>::max(),
        .executorZoneId  = std::numeric_limits<uint16>::max() - 1,
        .funcString      = "return xi.test()",
    };
    const auto endpoint = IPP{};
    int rerouteCalls{};
    int lookupCalls{};
    int sendCalls{};
    uint16 lookedUpZone{};
    IPP sentEndpoint{};
    ipc::LuaFunction sentMessage{};

    worldipc::HandleLuaFunction(
        message,
        [&](uint16 zoneId, const ipc::LuaFunction& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToZoneId(
                zoneId,
                delivered,
                [&](uint16 selectedZone) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpZone = selectedZone;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::LuaFunction& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved executor routed once") &&
           expect(lookedUpZone == message.executorZoneId, "executor zone selected") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "all-zero endpoint is valid") &&
           expect(sameMessage(sentMessage, message), "full payload forwarded unchanged");
}

auto testMissingExecutorZone() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    bool selectedZeroZone{};
    worldipc::HandleLuaFunction(
        ipc::LuaFunction{},
        [&](uint16 zoneId, const ipc::LuaFunction& delivered)
        {
            worldipc::RerouteMessageToZoneId(
                zoneId,
                delivered,
                [&](uint16 selectedZone) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    selectedZeroZone = selectedZone == 0;
                    return std::nullopt;
                },
                [&](const IPP&, const ipc::LuaFunction&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1 && selectedZeroZone, "zero executor zone looked up once") &&
           expect(sendCalls == 0, "missing executor sends nothing");
}

} // namespace

auto runWorldLuaFunction1279SelfTests() -> bool
{
    return testResolvedExecutorZone() && testMissingExecutorZone();
}
