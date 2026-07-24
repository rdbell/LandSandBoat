#include "test_map_main_host_6375.h"

#include "map/map_app_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map main host 6375 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for Map main / Application run host residuals (slice 6375).
// Go host halves: mapapp.NewMapApplication, MapMain, MapApplicationRun.
auto runMapMainHost6375SelfTests() -> bool
{
    using mapapp::FormatMapFatalException;
    using mapapp::MapBacktraceCommandName;
    using mapapp::MapGMCommandName;
    using mapapp::MapMainExitCode;
    using mapapp::MapReloadRecipesCommandName;
    using mapapp::MapServerName;
    using mapapp::MapStatsCommandName;
    using mapapp::ShouldTerminateOnNullEngine;

    bool ok = true;

    ok = expect(std::string(MapServerName) == "map", "server name") && ok;
    ok = expect(MapMainExitCode(true) == 0, "success exit 0") && ok;
    ok = expect(MapMainExitCode(false) == 1, "failure exit 1") && ok;
    ok = expect(ShouldTerminateOnNullEngine(false), "null engine terminate") && ok;
    ok = expect(!ShouldTerminateOnNullEngine(true), "engine present") && ok;
    ok = expect(FormatMapFatalException("boom") == "Fatal Exception: boom", "fatal format") && ok;

    ok = expect(std::string(MapGMCommandName) == "gm", "gm cmd") && ok;
    ok = expect(std::string(MapReloadRecipesCommandName) == "reload_recipes", "reload cmd") && ok;
    ok = expect(std::string(MapStatsCommandName) == "stats", "stats cmd") && ok;
    ok = expect(std::string(MapBacktraceCommandName) == "backtrace", "backtrace cmd") && ok;

    return ok;
}
