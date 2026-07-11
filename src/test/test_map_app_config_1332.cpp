#include "test_map_app_config_1332.h"

#include "map/map_app_config.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map app config 1332 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapAppConfig1332SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string(mapapp::MapServerName) == "map", "server name") && ok;
    ok = expect(mapapp::MapCLIArgumentCount == 4, "arg count") && ok;

    ok = expect(mapapp::MapCLIArguments[0].name == "--ip", "ip name") && ok;
    ok = expect(mapapp::MapCLIArguments[0].description == "Specify the IP address to bind to", "ip help") && ok;
    ok = expect(mapapp::MapCLIArguments[0].type == ArgumentType::Simple, "ip type") && ok;

    ok = expect(mapapp::MapCLIArguments[1].name == "--port", "port name") && ok;
    ok = expect(mapapp::MapCLIArguments[1].type == ArgumentType::Simple, "port type") && ok;

    ok = expect(mapapp::MapCLIArguments[2].name == "--lazy", "lazy name") && ok;
    ok = expect(mapapp::MapCLIArguments[2].description == "Load zones on demand. For development only.", "lazy help") && ok;
    ok = expect(mapapp::MapCLIArguments[2].type == ArgumentType::Flag, "lazy type") && ok;

    ok = expect(mapapp::MapCLIArguments[3].name == "--rebuild-navmeshes", "nav name") && ok;
    ok = expect(mapapp::MapCLIArguments[3].type == ArgumentType::Flag, "nav type") && ok;

    const auto built = mapapp::BuildArgumentDefinitions();
    ok               = expect(built.size() == 4, "built size") && ok;
    ok               = expect(built[2].name == "--lazy" && built[2].type == ArgumentType::Flag, "built lazy") && ok;

    ok = expect(std::string(mapapp::MapGMCommandName) == "gm", "gm cmd") && ok;
    ok = expect(std::string(mapapp::MapGMCommandHelp) == "Change a character's GM level", "gm help") && ok;
    ok = expect(std::string(mapapp::MapReloadRecipesCommandName) == "reload_recipes", "reload cmd") && ok;
    ok = expect(std::string(mapapp::MapStatsCommandName) == "stats", "stats cmd") && ok;
    ok = expect(std::string(mapapp::MapStatsCommandHelp) == "Print runtime stats", "stats help") && ok;
    ok = expect(std::string(mapapp::MapBacktraceCommandName) == "backtrace", "bt cmd") && ok;

    return ok;
}
