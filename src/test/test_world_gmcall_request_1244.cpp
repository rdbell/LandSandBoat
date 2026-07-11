#include "test_world_gmcall_request_1244.h"

#include "world/gmcall_request.h"

#include <iostream>
#include <limits>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "world GM-call request 1244 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testCanonicalRequestLog() -> bool
{
    const ipc::GMCallRequest request{
        .callId   = 42,
        .charId   = 77,
        .charName = "Prishe",
        .accId    = 91,
        .zoneId   = 255,
        .message  = "Help is needed",
    };
    return expect(world::gmcall::FormatRequestLog(request) ==
                      "GM Call #42 from Prishe (charId: 77, accId: 91, zone: 255): Help is needed",
                  "canonical log");
}

auto testUnsignedBoundariesAndUntrustedText() -> bool
{
    const ipc::GMCallRequest request{
        .callId   = std::numeric_limits<uint32>::max(),
        .charId   = std::numeric_limits<uint32>::max() - 1,
        .charName = "{name}%s",
        .accId    = std::numeric_limits<uint32>::max() - 2,
        .zoneId   = std::numeric_limits<uint16>::max(),
        .message  = "{} %n {message}",
    };
    return expect(world::gmcall::FormatRequestLog(request) ==
                      "GM Call #4294967295 from {name}%s (charId: 4294967294, accId: 4294967293, zone: 65535): {} %n {message}",
                  "unsigned boundaries and untrusted text");
}

auto testEmbeddedNullAndIgnoredFields() -> bool
{
    ipc::GMCallRequest request{
        .callId     = 1,
        .charId     = 2,
        .charName   = std::string{ "A\0B", 3 },
        .accId      = 3,
        .zoneId     = 4,
        .posX       = 1.25F,
        .posY       = -2.5F,
        .posZ       = 3.75F,
        .message    = std::string{ "M\0N", 3 },
        .parameters = { { "GMCALL.INPUT", "ignored" } },
    };
    const auto expected = std::string("GM Call #1 from A") + std::string("\0", 1) +
                          "B (charId: 2, accId: 3, zone: 4): M" + std::string("\0", 1) + "N";
    const auto first    = world::gmcall::FormatRequestLog(request);

    request.posX       = 999.0F;
    request.posY       = 888.0F;
    request.posZ       = 777.0F;
    request.parameters = { { "different", "values" } };
    const auto second  = world::gmcall::FormatRequestLog(request);

    return expect(first == expected, "embedded NUL preserved") &&
           expect(second == expected, "position and parameters ignored");
}

} // namespace

auto runWorldGMCallRequest1244SelfTests() -> bool
{
    bool ok = true;
    ok      = testCanonicalRequestLog() && ok;
    ok      = testUnsignedBoundariesAndUntrustedText() && ok;
    ok      = testEmbeddedNullAndIgnoredFields() && ok;
    return ok;
}
