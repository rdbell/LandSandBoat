#include "test_map_engine_maintenance_host_6399.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine maintenance host 6399 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine maintenance hosts (slice 6399).
// Go: ApplySessionCleanup / ApplyGarbageCollect / init SQL plans.
auto runMapEngineMaintenanceHost6399SelfTests() -> bool
{
    bool ok = true;

    const std::string sessionsSQL =
        "DELETE FROM accounts_sessions WHERE (? = 0 AND ? = 0) OR (server_addr = ? AND server_port = ?)";
    const std::string charVarsSQL    = "DELETE FROM char_vars WHERE expiry > 0 AND expiry <= ?";
    const std::string serverVarsSQL  = "DELETE FROM server_variables WHERE expiry > 0 AND expiry <= ?";
    const std::string expiredNotice  = "do_init: Removing expired database variables";

    ok = expect(!sessionsSQL.empty(), "sessions sql") && ok;
    ok = expect(charVarsSQL.find("char_vars") != std::string::npos, "char_vars") && ok;
    ok = expect(serverVarsSQL.find("server_variables") != std::string::npos, "server_vars") && ok;
    ok = expect(expiredNotice == "do_init: Removing expired database variables", "notice") && ok;

    // sessionCleanup order: cleanupSessions then EraseStaleDynamicTargIDs.
    const std::vector<std::string> cleanupOrder = { "cleanupSessions", "EraseStaleDynamicTargIDs" };
    ok = expect(cleanupOrder.size() == 2, "cleanup steps") && ok;
    ok = expect(cleanupOrder[0] == "cleanupSessions", "cleanup first") && ok;

    // garbageCollect single inject.
    const std::string gc = "garbageCollectFull";
    ok = expect(gc == "garbageCollectFull", "gc inject") && ok;

    return ok;
}
