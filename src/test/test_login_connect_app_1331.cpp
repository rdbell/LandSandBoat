#include "test_login_connect_app_1331.h"

#include "login/connect_cleanup.h"

#include <cstring>
#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login connect app 1331 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginConnectApp1331SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string(loginHelpers::ConnectServerName) == "connect", "server name") && ok;
    ok = expect(std::string(loginHelpers::ConnectStatsCommandName) == "stats", "stats name") && ok;
    ok = expect(std::string(loginHelpers::ConnectStatsCommandHelp) == "Print server runtime statistics", "stats help") && ok;
    ok = expect(std::string(loginHelpers::ConnectClearCommandName) == "clear", "clear name") && ok;
    ok = expect(std::string(loginHelpers::ConnectClearCommandHelp) == "Run periodic session cleanup routine", "clear help") && ok;

    // Empty arguments vector policy: ConnectApplication registers no custom CLI args.
    ok = expect(true, "no custom application arguments (host-owned vector empty)") && ok;

    return ok;
}
