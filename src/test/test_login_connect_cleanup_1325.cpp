#include "test_login_connect_cleanup_1325.h"

#include "common/cbasetypes.h"
#include "common/ipp.h"
#include "common/timer.h"
#include "login/connect_cleanup.h"

#include <chrono>
#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login connect cleanup 1325 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginConnectCleanup1325SelfTests() -> bool
{
    using namespace std::chrono_literals;
    bool ok = true;

    ok = expect(loginHelpers::SessionCleanInterval == 15min, "clean interval 15min") && ok;

    // Idle erase requires both peers null and expired.
    ok = expect(loginHelpers::ShouldEraseIdleSession(false, false, true), "idle expired erase") && ok;
    ok = expect(!loginHelpers::ShouldEraseIdleSession(true, false, true), "data peer keeps") && ok;
    ok = expect(!loginHelpers::ShouldEraseIdleSession(false, true, true), "view peer keeps") && ok;
    ok = expect(!loginHelpers::ShouldEraseIdleSession(true, true, true), "both peers keep") && ok;
    ok = expect(!loginHelpers::ShouldEraseIdleSession(false, false, false), "not expired keeps") && ok;

    const auto base = timer::time_point{};
    ok              = expect(!loginHelpers::IsSessionExpired(base, base), "exact start not expired") && ok;
    ok              = expect(!loginHelpers::IsSessionExpired(base + 15min, base), "exact 15min not expired (strict >)") && ok;
    ok              = expect(loginHelpers::IsSessionExpired(base + 15min + 1ms, base), "past 15min expired") && ok;
    ok              = expect(!loginHelpers::IsSessionExpired(base + 14min, base), "under 15min") && ok;

    ok = expect(loginHelpers::ShouldEraseEmptyIPEntry(true), "empty ip erase") && ok;
    ok = expect(!loginHelpers::ShouldEraseEmptyIPEntry(false), "nonempty ip keep") && ok;

    ok = expect(loginHelpers::FormatZMQEndpointString("tcp", "127.0.0.1", 54001) == "tcp://127.0.0.1:54001",
                "zmq endpoint") &&
         ok;
    ok = expect(loginHelpers::FormatZMQEndpointString("ipc", "path", 0) == "ipc://path:0", "zmq endpoint zero port") && ok;

    const uint32 authIP   = str2ip("10.0.0.5");
    const uint16 authPort = 54230;
    ok                    = expect(loginHelpers::ConnectDealerRoutingID(authIP, authPort) == IPP(authIP, authPort).getRawIPP(),
                "routing id matches IPP") &&
         ok;
    ok = expect(loginHelpers::ConnectDealerRoutingID(0, 0) == IPP(0, 0).getRawIPP(), "routing id zero") && ok;

    return ok;
}
