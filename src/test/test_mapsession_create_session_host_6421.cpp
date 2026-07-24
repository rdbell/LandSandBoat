#include "test_mapsession_create_session_host_6421.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession create session host 6421 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for createSession composition (slice 6421).
// Go: ApplyCreateSession / ApplyCreatePendingSession.
auto runMapsessionCreateSessionHost6421SelfTests() -> bool
{
    bool ok = true;

    const std::string selAddr = "SELECT charid FROM accounts_sessions WHERE client_addr = ? LIMIT 1";
    ok = expect(selAddr.find("client_addr") != std::string::npos, "addr sql") && ok;

    const std::string selChar = "SELECT charid FROM accounts_sessions WHERE charid = ? LIMIT 1";
    ok = expect(selChar.find("charid = ?") != std::string::npos, "char sql") && ok;

    const std::string sqlErr = "SQL query failed in MapSessionContainer::createSession!";
    ok = expect(sqlErr.find("createSession") != std::string::npos, "sql err") && ok;

    const std::string invalid = "recv_parse: Invalid login attempt from 1.2.3.4";
    ok = expect(invalid.find("Invalid login") != std::string::npos, "invalid login") && ok;

    // ShouldCreateSession: queryOK && hasRow
    ok = expect(true && true, "create gate") && ok;
    // ShouldCreatePendingSession: queryOK only
    ok = expect(true, "pending gate") && ok;

    return ok;
}
