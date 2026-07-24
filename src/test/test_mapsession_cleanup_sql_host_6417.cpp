#include "test_mapsession_cleanup_sql_host_6417.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession cleanup sql host 6417 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for cleanupSessions SQL (slice 6417).
// Go: ApplyTimeoutCleanupSQL / OtherMapFromServer / SQL constants.
auto runMapsessionCleanupSqlHost6417SelfTests() -> bool
{
    bool ok = true;

    const std::string del = "DELETE FROM accounts_sessions WHERE charid = ?";
    ok = expect(del.find("accounts_sessions") != std::string::npos, "delete sql") && ok;

    const std::string sel = "SELECT server_addr, server_port FROM accounts_sessions WHERE charid = ?";
    ok = expect(sel.find("server_addr") != std::string::npos, "select sql") && ok;

    const std::string setDisc = "UPDATE char_flags SET disconnecting = 1 WHERE charid = ?";
    ok = expect(setDisc.find("disconnecting = 1") != std::string::npos, "set disc") && ok;

    const std::string clrDisc = "UPDATE char_flags SET disconnecting = 0 WHERE charid = ?";
    ok = expect(clrDisc.find("disconnecting = 0") != std::string::npos, "clr disc") && ok;

    auto otherMap = [](uint32_t mapIP, uint32_t mapPort, uint32_t serverAddr, uint32_t serverPort) -> bool {
        return (mapIP != 0 && serverAddr != mapIP) || (mapPort != 0 && serverPort != mapPort);
    };
    ok = expect(otherMap(1, 0, 2, 0) && !otherMap(1, 2, 1, 2), "otherMap") && ok;
    ok = expect(!otherMap(0, 0, 9, 9), "zero map ipp") && ok;

    const std::string without = "map_cleanup: WITHOUT CHAR timed out, session closed on this process";
    ok = expect(without.find("WITHOUT CHAR") != std::string::npos, "without char") && ok;

    return ok;
}
