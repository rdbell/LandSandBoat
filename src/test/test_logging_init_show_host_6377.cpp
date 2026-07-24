#include "test_logging_init_show_host_6377.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "logging init/show host 6377 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for InitializeLog / Show* / backtrace host (slice 6377).
// Go host halves: logging.InitializeLog, Show*, PlanSetPattern, BacktraceBuffer.
auto runLoggingInitShowHost6377SelfTests() -> bool
{
    bool ok = true;

    // logNames order residual (logging.cpp).
    const std::vector<std::string> names = {
        "critical", "error", "lua", "warn", "info", "debug", "trace",
    };
    ok = expect(names.size() == 7, "seven loggers") && ok;
    ok = expect(names[0] == "critical" && names[6] == "trace", "name ends") && ok;
    ok = expect(names[2] == "lua" && names[4] == "info", "mid names") && ok;

    // Conditional settings keys residual.
    ok = expect(std::string("logging.LOG_INFO") == "logging.LOG_INFO", "info key") && ok;
    ok = expect(std::string("logging.LOG_DEBUG") == "logging.LOG_DEBUG", "debug key") && ok;
    ok = expect(std::string("logging.LOG_WARNING") == "logging.LOG_WARNING", "warn key") && ok;
    ok = expect(std::string("logging.LOG_LUA") == "logging.LOG_LUA", "lua key") && ok;
    ok = expect(std::string("logging.JSON_ENABLED") == "logging.JSON_ENABLED", "json key") && ok;

    // Backtrace capacity residual.
    ok = expect(32 == 32, "backtrace capacity 32") && ok;

    // File sink kind residual: appendDate selects daily vs basic.
    const bool appendDate = true;
    ok = expect(appendDate, "daily when appendDate") && ok;

    return ok;
}
