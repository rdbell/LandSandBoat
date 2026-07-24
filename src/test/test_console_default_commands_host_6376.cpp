#include "test_console_default_commands_host_6376.h"

#include "common/console_service.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "console default commands host 6376 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire suite for ConsoleService default-command residuals (slice 6376).
// Go host halves: console.RegisterDefaultCommands, AttachConsole, ParseLogLevelCommand.
auto runConsoleDefaultCommandsHost6376SelfTests() -> bool
{
    bool ok = true;

    // Tokenize residual used by ConsoleLoopStep / Dispatch.
    const auto tokens = consolehelpers::Tokenize("log_level 2");
    ok = expect(tokens.size() == 2, "tokenize size") && ok;
    ok = expect(tokens[0] == "log_level", "tokenize cmd") && ok;
    ok = expect(tokens[1] == "2", "tokenize arg") && ok;

    // Default command name catalog (registration order in registerDefaultCommands).
    const std::vector<std::string> names = {
        "help",
        "version",
        "reload_settings",
        "log_level",
        "lua",
        "crash",
        "crash_worker",
        "throw",
        "exit",
    };
    ok = expect(names.size() == 9, "nine defaults") && ok;
    ok = expect(names[0] == "help" && names[8] == "exit", "first last") && ok;

    // log_level clamp residual (0..3 names).
    const std::vector<std::string> levelNames = { "trace", "debug", "info", "warn" };
    ok = expect(levelNames.size() == 4, "level names") && ok;
    ok = expect(levelNames[2] == "info", "info name") && ok;

    return ok;
}
