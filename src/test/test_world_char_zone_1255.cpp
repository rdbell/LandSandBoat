#include "test_world_char_zone_1255.h"

#include "world/char_zone.h"

#include "common/ipp.h"
#include "common/types/maybe.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world CharZone 1255 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testZoneExitRemovesOnly() -> bool
{
    int    removeCalls{};
    int    lookupCalls{};
    int    updateCalls{};
    int    sendCalls{};
    uint32 removedId{};

    worldipc::HandleCharZone(
        ipc::CharZone{ .charId = 0, .destinationZoneId = 0xFFFF },
        [&](const uint32 charId)
        {
            ++removeCalls;
            removedId = charId;
        },
        [&](const uint16) -> Maybe<IPP>
        {
            ++lookupCalls;
            return std::nullopt;
        },
        [&](const uint32, const IPP&)
        {
            ++updateCalls;
        },
        [&](const uint16, const ipc::CharZone&)
        {
            ++sendCalls;
        });

    return expect(removeCalls == 1 && removedId == 0, "exit removes exact zero character ID") &&
           expect(lookupCalls == 0 && updateCalls == 0 && sendCalls == 0, "exit short-circuits all other effects");
}

auto testFirstLookupMissDoesNothing() -> bool
{
    int lookupCalls{};
    int removeCalls{};
    int updateCalls{};
    int rerouteCalls{};
    uint16 lookedUpZone{};

    worldipc::HandleCharZone(
        ipc::CharZone{ .charId = 7, .destinationZoneId = 0 },
        [&](const uint32)
        {
            ++removeCalls;
        },
        [&](const uint16 zoneId) -> Maybe<IPP>
        {
            ++lookupCalls;
            lookedUpZone = zoneId;
            return std::nullopt;
        },
        [&](const uint32, const IPP&)
        {
            ++updateCalls;
        },
        [&](const uint16, const ipc::CharZone&)
        {
            ++rerouteCalls;
        });

    return expect(lookupCalls == 1 && lookedUpZone == 0, "first miss looks up exact zero zone once") &&
           expect(removeCalls == 0 && updateCalls == 0 && rerouteCalls == 0, "first miss has no remove/cache/reroute effects");
}

auto testSecondLookupCanChangeOrMiss() -> bool
{
    const auto cached = IPP(0x0100007F, 54230);
    const auto routed = IPP(0x0200007F, 54231);
    bool       ok     = true;

    for (const bool secondPresent : { true, false })
    {
        int              lookupCalls{};
        int              removeCalls{};
        uint32           updatedId{};
        IPP              updatedEndpoint{};
        int              updateCalls{};
        int              sendCalls{};
        IPP              sentEndpoint{};
        ipc::CharZone    sentMessage{};
        std::vector<char> order{};
        const auto message = ipc::CharZone{ .charId = 0, .destinationZoneId = 0 };

        auto lookup = [&](const uint16 zoneId) -> Maybe<IPP>
        {
            order.push_back('L');
            ++lookupCalls;
            if (zoneId != message.destinationZoneId)
            {
                return std::nullopt;
            }
            if (lookupCalls == 1)
            {
                return cached;
            }
            return secondPresent ? Maybe<IPP>{ routed } : std::nullopt;
        };

        worldipc::HandleCharZone(
            message,
            [&](const uint32)
            {
                ++removeCalls;
            },
            lookup,
            [&](const uint32 charId, const IPP& endpoint)
            {
                order.push_back('U');
                ++updateCalls;
                updatedId       = charId;
                updatedEndpoint = endpoint;
            },
            [&](const uint16 zoneId, const ipc::CharZone& delivered)
            {
                if (const auto endpoint = lookup(zoneId))
                {
                    order.push_back('S');
                    ++sendCalls;
                    sentEndpoint = *endpoint;
                    sentMessage  = delivered;
                }
            });

        ok = expect(lookupCalls == 2, "successful first lookup triggers second lookup") && ok;
        ok = expect(updateCalls == 1 && updatedId == message.charId && updatedEndpoint.getRawIPP() == cached.getRawIPP(),
                    "cache uses first lookup endpoint") &&
             ok;
        ok = expect(removeCalls == 0, "non-exit never removes cache entry") && ok;
        if (secondPresent)
        {
            ok = expect(sendCalls == 1 && sentEndpoint.getRawIPP() == routed.getRawIPP() && sentMessage.charId == message.charId &&
                            sentMessage.destinationZoneId == message.destinationZoneId,
                        "send uses changed second endpoint and unchanged payload") &&
                 ok;
            ok = expect(order == std::vector<char>{ 'L', 'U', 'L', 'S' }, "present operation order") && ok;
        }
        else
        {
            ok = expect(sendCalls == 0, "second miss suppresses send") && ok;
            ok = expect(order == std::vector<char>{ 'L', 'U', 'L' }, "missing operation order") && ok;
        }
    }
    return ok;
}

} // namespace

auto runWorldCharZone1255SelfTests() -> bool
{
    return testZoneExitRemovesOnly() && testFirstLookupMissDoesNothing() && testSecondLookupCanChangeOrMiss();
}
