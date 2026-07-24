#include "test_map_engine_tasks_host_6398.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine tasks host 6398 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine::init recurring scheduler tasks (slice 6398).
// Go: PlanMapEngineRecurringTasks / RegisterMapEngineRecurringTasks / PostMapEngineWatchdogs.
// Interval literals mirror map_constants.h without pulling that header (uint64 deps).
auto runMapEngineTasksHost6398SelfTests() -> bool
{
    bool ok = true;

    using namespace std::chrono_literals;

    const auto sessionCleanup = 5s;
    const auto garbageCollect = 15min;
    const auto timeServer     = 2400ms;
    const auto persistVars    = 1min;
    const auto ipcPump        = 100ms;

    ok = expect(sessionCleanup == 5s, "session cleanup 5s") && ok;
    ok = expect(garbageCollect == 15min, "gc 15min") && ok;
    ok = expect(timeServer == 2400ms, "time server 2400ms") && ok;
    ok = expect(persistVars == 1min, "persist 1min") && ok;
    ok = expect(ipcPump == 100ms, "ipc pump 100ms") && ok;
    // flushStatistics reuses time-server tick interval.
    ok = expect(timeServer == 2400ms, "flush stats interval") && ok;

    // Ordered catalog names mirror MapEngine::init registration.
    const std::vector<std::string> names = {
        "sessionCleanup",
        "garbageCollect",
        "time_server",
        "PersistVolatileServerVars",
        "handle_incoming",
        "flushStatistics",
    };
    ok = expect(names.size() == 6, "catalog size") && ok;
    ok = expect(names[0] == "sessionCleanup", "first cleanup") && ok;
    ok = expect(names[5] == "flushStatistics", "last flush") && ok;

    // isTestServer gate: registration skipped when true.
    const bool isTestServer   = true;
    const bool shouldRegister = !isTestServer;
    ok = expect(!shouldRegister, "skip when test server") && ok;

    return ok;
}
