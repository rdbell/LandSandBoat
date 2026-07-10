#include "test_world_gmcall_response_1240.h"

#include "world/gmcall_response.h"

#include <iostream>
#include <string>
#include <utility>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world GM-call response 1240 self-test failed: " << label << '\n';
    }
    return condition;
}

auto responseWith(std::string message) -> ipc::GMCallResponse
{
    return ipc::GMCallResponse{
        .callId  = 0x11223344,
        .charId  = 0x55667788,
        .message = std::move(message),
    };
}

auto testShortAndExactResponsesAreUnchanged() -> bool
{
    std::string exactMessage(world::gmcall::MaxResponseBytes, 'x');
    exactMessage.front()                               = 'a';
    exactMessage[world::gmcall::MaxResponseBytes / 2U] = 'm';
    exactMessage.back()                                = 'z';

    const auto shortResponse = world::gmcall::TruncateResponse(responseWith("help is coming"));
    const auto exactResponse = world::gmcall::TruncateResponse(responseWith(exactMessage));

    return expect(shortResponse.callId == 0x11223344 && shortResponse.charId == 0x55667788 && shortResponse.message == "help is coming",
                  "short response and identifiers") &&
           expect(exactResponse.callId == 0x11223344 && exactResponse.charId == 0x55667788 && exactResponse.message == exactMessage,
                  "exact response and identifiers");
}

auto testLongResponseIsTruncatedByBytes() -> bool
{
    auto message = std::string(world::gmcall::MaxResponseBytes - 1, 'a');
    message += "\xC3\xA9"; // UTF-8 é crosses the truncation boundary.

    const auto response = world::gmcall::TruncateResponse(responseWith(message));
    return expect(response.message.size() == world::gmcall::MaxResponseBytes, "long response size") &&
           expect(static_cast<unsigned char>(response.message.back()) == 0xC3, "byte-oriented UTF-8 split");
}

auto testEmbeddedNullIsPreserved() -> bool
{
    std::string message(world::gmcall::MaxResponseBytes + 1, 'b');
    message[100] = '\0';

    const auto response = world::gmcall::TruncateResponse(responseWith(message));
    return expect(response.message.size() == world::gmcall::MaxResponseBytes, "embedded NUL response size") &&
           expect(response.message[100] == '\0' && response.message[101] == 'b', "embedded NUL preserved");
}

} // namespace

auto runWorldGMCallResponse1240SelfTests() -> bool
{
    bool ok = true;
    ok      = expect(world::gmcall::MaxResponseBytes == 1024, "maximum response bytes") && ok;
    ok      = testShortAndExactResponsesAreUnchanged() && ok;
    ok      = testLongResponseIsTruncatedByBytes() && ok;
    ok      = testEmbeddedNullIsPreserved() && ok;
    return ok;
}
