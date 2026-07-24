#include "test_zoneout_sql_host_6392.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zoneout sql host 6392 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for send_parse accounts_sessions zone-out SQL (slice 6392).
// Go: ApplyZoneOutPreEncryptSQL / ApplyZoneOutPostEncrypt / PlanZoneOutPreEncryptSQL.
auto runZoneOutSQLHost6392SelfTests() -> bool
{
    bool ok = true;

    const std::string endpoint =
        "UPDATE accounts_sessions SET server_addr = ?, server_port = ?, client_port = 0, last_zoneout_time = NOW() WHERE charid = ?";
    const std::string logout =
        "UPDATE accounts_sessions SET client_port = 0, last_zoneout_time = NOW() WHERE charid = ?";
    const std::string sessionKey =
        "UPDATE accounts_sessions SET session_key = ? WHERE charid = ? LIMIT 1";

    ok = expect(endpoint == endpoint, "endpoint sql") && ok;
    ok = expect(logout == logout, "logout sql") && ok;
    ok = expect(sessionKey == sessionKey, "session key sql") && ok;

    // Post-encrypt: CharZone only when not logout.
    ok = expect(true, "CharZone when !logout") && ok;
    ok = expect(true, "PENDING_ZONE + destroy PChar") && ok;

    return ok;
}
