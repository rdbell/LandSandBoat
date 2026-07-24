#include "test_connect_main_host_6372.h"

#include "login/connect_cleanup.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "connect main host 6372 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Connect main / Application run host residuals (slice 6372).
// Go host halves: loginsession.NewConnectApplication, ConnectMain, ConnectClearCommand.
auto runConnectMainHost6372SelfTests() -> bool
{
    using loginHelpers::ConnectClearCommandHelp;
    using loginHelpers::ConnectClearCommandName;
    using loginHelpers::ConnectMainExitCode;
    using loginHelpers::ConnectServerName;
    using loginHelpers::ConnectStatsCommandHelp;
    using loginHelpers::ConnectStatsCommandName;
    using loginHelpers::FormatConnectStats;
    using loginHelpers::ShouldEraseEmptyIPEntry;
    using loginHelpers::ShouldEraseOnClearCommand;

    bool ok = true;

    ok = expect(std::string(ConnectServerName) == "connect", "server name") && ok;
    ok = expect(ConnectMainExitCode == 0, "main exit 0") && ok;
    ok = expect(std::string(ConnectStatsCommandName) == "stats", "stats name") && ok;
    ok = expect(std::string(ConnectClearCommandName) == "clear", "clear name") && ok;
    ok = expect(std::string(ConnectStatsCommandHelp) == "Print server runtime statistics", "stats help") && ok;
    ok = expect(std::string(ConnectClearCommandHelp) == "Run periodic session cleanup routine", "clear help") && ok;

    // Clear gates used by ConnectClearCommand host.
    ok = expect(ShouldEraseOnClearCommand(false, false, true), "clear erase idle") && ok;
    ok = expect(!ShouldEraseOnClearCommand(true, false, true), "clear keep data") && ok;
    ok = expect(!ShouldEraseOnClearCommand(false, true, true), "clear keep view") && ok;
    ok = expect(!ShouldEraseOnClearCommand(false, false, false), "clear keep not expired") && ok;
    ok = expect(ShouldEraseEmptyIPEntry(true), "empty IP erase") && ok;
    ok = expect(!ShouldEraseEmptyIPEntry(false), "non-empty IP keep") && ok;

    ok = expect(FormatConnectStats(2, 5) == "Serving 2 IP addresses with 5 accounts", "stats format") && ok;

    return ok;
}
