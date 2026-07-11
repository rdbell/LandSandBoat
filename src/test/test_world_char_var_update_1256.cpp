#include "test_world_char_var_update_1256.h"

#include "world/char_id_reroute.h"
#include "world/char_var_update.h"

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
        std::cerr << "world CharVarUpdate 1256 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testReroutesExactMessageByCharacterId() -> bool
{
    const ipc::CharVarUpdate message{
        .charId  = 0,
        .value   = std::numeric_limits<int32>::min(),
        .expiry  = std::numeric_limits<uint32>::max(),
        .varName = "",
    };
    int                rerouteCalls{};
    int                lookupCalls{};
    int                sendCalls{};
    uint32             routedId{1};
    IPP                routedEndpoint{};
    ipc::CharVarUpdate routedMessage{};
    const auto         endpoint = IPP(0x0100007F, 54230);

    worldipc::HandleCharVarUpdate(
        message,
        [&](const uint32 charId, const ipc::CharVarUpdate& delivered)
        {
            ++rerouteCalls;
            worldipc::RerouteMessageToCharId(
                charId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    routedId = targetId;
                    return endpoint;
                },
                [&](const IPP& target, const ipc::CharVarUpdate& sent)
                {
                    ++sendCalls;
                    routedEndpoint = target;
                    routedMessage  = sent;
                });
        });

    return expect(rerouteCalls == 1 && lookupCalls == 1 && sendCalls == 1 && routedId == 0,
                  "zero character ID looked up and sent exactly once") &&
           expect(routedEndpoint.getRawIPP() == endpoint.getRawIPP(), "resolved endpoint receives update") &&
           expect(routedMessage.charId == message.charId && routedMessage.value == message.value &&
                      routedMessage.expiry == message.expiry && routedMessage.varName == message.varName,
                  "extreme and empty payload forwarded unchanged");
}

auto testMissingCharacterDoesNotSend() -> bool
{
    int lookupCalls{};
    int sendCalls{};

    worldipc::HandleCharVarUpdate(
        ipc::CharVarUpdate{ .charId = 7, .value = -1, .expiry = 2, .varName = "missing" },
        [&](const uint32 charId, const ipc::CharVarUpdate& delivered)
        {
            worldipc::RerouteMessageToCharId(
                charId,
                delivered,
                [&](const uint32 targetId) -> Maybe<IPP>
                {
                    ++lookupCalls;
                    return targetId == 7 ? std::nullopt : Maybe<IPP>{ IPP{} };
                },
                [&](const IPP&, const ipc::CharVarUpdate&)
                {
                    ++sendCalls;
                });
        });

    return expect(lookupCalls == 1, "missing character looked up exactly once") &&
           expect(sendCalls == 0, "missing character sends nothing");
}

auto testZeroValueAndExpiryAreForwarded() -> bool
{
    const ipc::CharVarUpdate message{ .charId = 9, .value = 0, .expiry = 0, .varName = "zero" };
    int                      rerouteCalls{};
    ipc::CharVarUpdate       routedMessage{};

    worldipc::HandleCharVarUpdate(
        message,
        [&](const uint32, const ipc::CharVarUpdate& delivered)
        {
            worldipc::RerouteMessageToCharId(
                message.charId,
                delivered,
                [](const uint32) -> Maybe<IPP>
                {
                    return IPP(0x0200007F, 54231);
                },
                [&](const IPP&, const ipc::CharVarUpdate& sent)
                {
                    ++rerouteCalls;
                    routedMessage = sent;
                });
        });

    return expect(rerouteCalls == 1 && routedMessage.value == 0 && routedMessage.expiry == 0 &&
                      routedMessage.varName == "zero",
                  "zero value and expiry forwarded without normalization");
}

} // namespace

auto runWorldCharVarUpdate1256SelfTests() -> bool
{
    return testReroutesExactMessageByCharacterId() && testMissingCharacterDoesNotSend() &&
           testZeroValueAndExpiryAreForwarded();
}
