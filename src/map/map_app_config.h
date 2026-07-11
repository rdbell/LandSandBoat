#pragma once

#include "common/arguments.h"
#include "common/cbasetypes.h"

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

// Pure MapApplication configuration constants extracted so native tests can
// pin CLI/console registration without Application/scheduler hosts.

namespace mapapp
{

// MapServerName is ApplicationConfig.serverName for MapApplication.
inline constexpr const char* MapServerName = "map";

// Map CLI argument catalog (order matches production appConfig()).
struct MapCLIArg
{
    std::string_view name;
    std::string_view description;
    ArgumentType     type;
};

inline constexpr std::array<MapCLIArg, 4> MapCLIArguments{ {
    { "--ip", "Specify the IP address to bind to", ArgumentType::Simple },
    { "--port", "Specify the port to bind to", ArgumentType::Simple },
    { "--lazy", "Load zones on demand. For development only.", ArgumentType::Flag },
    { "--rebuild-navmeshes", "Force rebuild all navmeshes from ximesh on startup.", ArgumentType::Flag },
} };

inline constexpr std::size_t MapCLIArgumentCount = MapCLIArguments.size();

// Console command registration names/help for MapApplication::registerCommands.
inline constexpr const char* MapGMCommandName               = "gm";
inline constexpr const char* MapGMCommandHelp               = "Change a character's GM level";
inline constexpr const char* MapReloadRecipesCommandName     = "reload_recipes";
inline constexpr const char* MapReloadRecipesCommandHelp     = "Reload crafting recipes";
inline constexpr const char* MapStatsCommandName            = "stats";
inline constexpr const char* MapStatsCommandHelp            = "Print runtime stats";
inline constexpr const char* MapBacktraceCommandName        = "backtrace";
inline constexpr const char* MapBacktraceCommandHelp        = "Print backtrace";

// BuildArgumentDefinitions returns the runtime ArgumentDefinition vector used by appConfig.
inline auto BuildArgumentDefinitions() -> std::vector<ArgumentDefinition>
{
    std::vector<ArgumentDefinition> arguments;
    arguments.reserve(MapCLIArguments.size());
    for (const auto& arg : MapCLIArguments)
    {
        arguments.push_back(ArgumentDefinition{
            .name        = std::string(arg.name),
            .description = std::string(arg.description),
            .type        = arg.type,
        });
    }
    return arguments;
}

} // namespace mapapp
