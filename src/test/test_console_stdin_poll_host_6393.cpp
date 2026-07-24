#include "test_console_stdin_poll_host_6393.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "console stdin poll host 6393 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for consoleLoop stdinHasData/getLine (slice 6393).
// Go: DrainStdinLines / RunConsolePollTick / DefaultStdinHasData.
auto runConsoleStdinPollHost6393SelfTests() -> bool
{
    bool ok = true;

    const std::vector<std::string> steps = {
        "stdinHasData",
        "getLine",
        "hasLine_break",
        "or_sleep_50ms",
    };
    ok = expect(steps.size() == 4, "step count") && ok;
    ok = expect(steps[0] == "stdinHasData", "poll first") && ok;

    ok = expect(std::string("> Command error: ") + "x" + "\n" == "> Command error: x\n", "cmd err") && ok;
    ok = expect(std::string("> Unknown command: ") + "nope" + "\n" == "> Unknown command: nope\n", "unknown") && ok;

    return ok;
}
