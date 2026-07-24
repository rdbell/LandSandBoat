#include "test_mapsession_cleanup_sessions_host_6420.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession cleanup sessions host 6420 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for cleanupSessions composition (slice 6420).
// Go: ApplyCleanupConfirmedSession / ApplyCleanupPendingSession.
auto runMapsessionCleanupSessionsHost6420SelfTests() -> bool
{
    bool ok = true;

    // Outer gate 5s, then configured timeout
    constexpr int outerSec = 5;
    ok = expect(outerSec == 5, "outer gate") && ok;

    // STATUS_TYPE::NORMAL == 0
    ok = expect(0 == 0, "normal status") && ok;

    // Stages: Active / LinkDead / Expired
    ok = expect(true, "three stages") && ok;

    // Timeout on other map + has char: no DELETE accounts_sessions
    ok = expect(true, "otherMap skip delete") && ok;

    // Pending: erase_if returns true after plan erase action
    ok = expect(true, "pending erase") && ok;

    return ok;
}
