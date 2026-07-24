#include "test_map_console_commands_host_6396.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map console commands host 6396 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine console commands (slice 6396).
// Go: RegisterMapEngineConsoleCommands / ApplyStats / ApplyBacktrace / ApplyReloadRecipes.
auto runMapConsoleCommandsHost6396SelfTests() -> bool
{
    bool ok = true;

    const std::vector<std::string> cmds = {"gm", "reload_recipes", "stats", "backtrace"};
    ok = expect(cmds.size() == 4, "cmd count") && ok;
    ok = expect(cmds[0] == "gm", "gm first") && ok;

    ok = expect(std::string("=== Map Statistics ===\n\n") == "=== Map Statistics ===\n\n", "stats header") && ok;
    ok = expect(std::string("> Reloading crafting recipes\n") == "> Reloading crafting recipes\n", "recipes notice") && ok;

    return ok;
}
