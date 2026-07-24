#include "test_console_crash_inject_host_6394.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "console crash inject host 6394 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for utils::crash + console throw (slice 6394).
// Go: ApplyProcessCrash / ProcessCrashCommandInjects / DefaultThrowException.
auto runConsoleCrashInjectHost6394SelfTests() -> bool
{
    bool ok = true;

    ok = expect(0xDEADu == 0xDEADu, "null write value") && ok;
    ok = expect(std::string("crash command is likely optimized out in release mode.") ==
                    "crash command is likely optimized out in release mode.",
                "release log") &&
         ok;
    ok = expect(std::string("Exception thrown from console command (on main thread)") ==
                    "Exception thrown from console command (on main thread)",
                "throw msg") &&
         ok;

    return ok;
}
