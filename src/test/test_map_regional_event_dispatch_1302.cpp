#include "test_map_regional_event_dispatch_1302.h"

#include "map/regional_event_dispatch.h"

#include <algorithm>
#include <iostream>
#include <span>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map regional event dispatch 1302 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testConquestForwardsTypeAndPayload() -> bool
{
    const std::vector<uint8> payload{ 0x00, 0x7F, 0x80, 0xFF, 0x01 };
    const ipc::ConquestEvent message{
        .type    = static_cast<ConquestMessage>(0xFE),
        .payload = payload,
    };

    int                         calls{};
    ConquestMessage             deliveredType{};
    std::span<const uint8>      deliveredPayload{};
    const uint8*                payloadData = message.payload.data();

    mapipc::HandleConquestEvent(
        message,
        [&](const ConquestMessage type, const std::span<const uint8> data)
        {
            ++calls;
            deliveredType    = type;
            deliveredPayload = data;
        });

    return expect(calls == 1, "conquest consumer called once") &&
           expect(deliveredType == static_cast<ConquestMessage>(0xFE), "conquest type preserved") &&
           expect(deliveredPayload.size() == payload.size(), "conquest payload size") &&
           expect(deliveredPayload.data() == payloadData, "conquest payload aliases message range") &&
           expect(std::equal(deliveredPayload.begin(), deliveredPayload.end(), payload.begin()), "conquest payload bytes");
}

auto testConquestEmptyPayloadAndZeroType() -> bool
{
    int             calls{};
    ConquestMessage deliveredType{ static_cast<ConquestMessage>(1) };
    std::size_t     deliveredSize{ 99 };

    mapipc::HandleConquestEvent(
        ipc::ConquestEvent{},
        [&](const ConquestMessage type, const std::span<const uint8> data)
        {
            ++calls;
            deliveredType = type;
            deliveredSize = data.size();
        });

    return expect(calls == 1, "zero conquest still delivered") &&
           expect(deliveredType == static_cast<ConquestMessage>(0), "zero type") &&
           expect(deliveredSize == 0, "empty payload size");
}

auto testRegionalNoOps() -> bool
{
    int sideEffects{};
    mapipc::HandleBesiegedEvent(
        ipc::BesiegedEvent{ .type = static_cast<BesiegedMessage>(1), .payload = { 1, 2 } },
        [&]()
        {
            ++sideEffects;
        });
    mapipc::HandleCampaignEvent(
        ipc::CampaignEvent{ .type = static_cast<CampaignMessage>(2), .payload = { 3 } },
        [&]()
        {
            ++sideEffects;
        });
    mapipc::HandleColonizationEvent(
        ipc::ColonizationEvent{ .type = static_cast<ColonizationMessage>(3), .payload = { 4, 5, 6 } },
        [&]()
        {
            ++sideEffects;
        });

    // The no-op seams accept unused trailing callables without invoking them.
    return expect(sideEffects == 0, "besieged/campaign/colonization remain no-ops");
}

} // namespace

auto runMapRegionalEventDispatch1302SelfTests() -> bool
{
    return testConquestForwardsTypeAndPayload() && testConquestEmptyPayloadAndZeroType() && testRegionalNoOps();
}
