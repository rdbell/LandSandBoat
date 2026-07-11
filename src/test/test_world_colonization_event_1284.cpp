#include "test_world_colonization_event_1284.h"

#include "world/colonization_event.h"

#include <iostream>
#include <limits>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world ColonizationEvent 1284 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testDispatch() -> bool
{
    const IPP source{ 0x04030201, std::numeric_limits<uint16>::max() };
    const ipc::ColonizationEvent message{
        .type = static_cast<ColonizationMessage>(std::numeric_limits<uint8>::max()),
        .payload = { 0x00, 0x7F, 0x80, 0xFF },
    };
    int calls{};
    ColonizationMessage deliveredType{};
    IPP deliveredSource{};
    std::vector<uint8> deliveredPayload{};
    worldipc::HandleColonizationEvent(
        source,
        message,
        [&](ColonizationMessage type, IPPMessage&& delivered)
        {
            ++calls;
            deliveredType = type;
            deliveredSource = delivered.ipp;
            deliveredPayload = delivered.payload;
            delivered.payload[0] = 0xEE;
            return false;
        });

    return expect(calls == 1, "stub subsystem called once") &&
           expect(static_cast<uint8>(deliveredType) == std::numeric_limits<uint8>::max(), "raw event type preserved") &&
           expect(deliveredSource.getRawIPP() == source.getRawIPP(), "source endpoint preserved") &&
           expect(deliveredPayload == message.payload, "full payload delivered") &&
           expect(message.payload[0] == 0x00, "subsystem receives owned payload copy");
}

} // namespace

auto runWorldColonizationEvent1284SelfTests() -> bool
{
    return testDispatch();
}
