#include "test_map_main_exit_1342.h"

#include "map/map_app_config.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map main exit 1342 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapMainExit1342SelfTests() -> bool
{
    bool ok = true;

    ok = expect(mapapp::MapMainExitCode(true) == 0, "success exit") && ok;
    ok = expect(mapapp::MapMainExitCode(false) == 1, "failure exit") && ok;

    ok = expect(mapapp::FormatMapFatalException("boom") == "Fatal Exception: boom", "fatal fmt") && ok;
    ok = expect(mapapp::FormatMapFatalException("") == "Fatal Exception: ", "fatal empty") && ok;

    ok = expect(mapapp::ShouldTerminateOnNullEngine(false), "null engine terminate") && ok;
    ok = expect(!mapapp::ShouldTerminateOnNullEngine(true), "engine ok") && ok;

    return ok;
}
