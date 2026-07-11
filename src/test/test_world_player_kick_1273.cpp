#include "test_world_player_kick_1273.h"

#include "world/char_id_reroute.h"
#include "world/player_kick.h"

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
        std::cerr << "world PlayerKick 1273 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testResolvedVictim() -> bool
{
    const ipc::PlayerKick message{ .victimId = std::numeric_limits<uint32>::max() };
    const auto            endpoint = IPP{};
    int                   rerouteCalls{};
    int                   lookupCalls{};
    int                   sendCalls{};
    uint32                lookedUpId{};
    IPP                   sentEndpoint{};
    ipc::PlayerKick       sentMessage{};

    worldipc::HandlePlayerKick(
        message,
        [&](const uint32 victimId, const ipc::PlayerKick& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharId(
                victimId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    lookedUpId = targetId;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::PlayerKick& sent)
                {
                    ++sendCalls;
                    sentEndpoint = target;
                    sentMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1, "resolved victim routed once") &&
           expect(lookedUpId == message.victimId, "victim ID selected for lookup") &&
           expect(sentEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved all-zero endpoint receives kick") &&
           expect(sentMessage.victimId == message.victimId, "kick payload forwarded unchanged");
}

auto testMissingVictim() -> bool
{
    int lookupCalls{};
    int sendCalls{};
    worldipc::HandlePlayerKick(
        ipc::PlayerKick{},
        [&](const uint32 victimId, const ipc::PlayerKick& delivered)
        {
            worldipc::RerouteMessageToCharId(
                victimId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 0 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::PlayerKick&) { ++sendCalls; });
        });

    return expect(lookupCalls == 1, "zero victim looked up once") &&
           expect(sendCalls == 0, "missing victim sends nothing");
}

} // namespace

auto runWorldPlayerKick1273SelfTests() -> bool
{
    return testResolvedVictim() && testMissingVictim();
}
