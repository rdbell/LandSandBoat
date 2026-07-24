#pragma once

#include "common/arguments.h"
#include "common/cbasetypes.h"

#include <array>
#include <cstddef>
#include <fmt/format.h>
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

// MapEngineConfigInputs are the pure values read from CLI/settings before
// constructing MapConfig / IPP for MapEngine.
struct MapEngineConfigInputs
{
    uint32 ip{};               // network-order from str2ip; 0 when --ip absent
    uint16 port{};             // 0 when --port absent
    bool   inCI{};             // Application::isRunningInCI()
    bool   lazyZones{};        // --lazy
    bool   rebuildNavmeshes{}; // --rebuild-navmeshes
};

// ResolveMapBindIP mirrors: present(--ip) ? str2ip(*maybe) : 0.
// hasIP is whether --ip was present; parsedIP is host-evaluated str2ip result.
inline auto ResolveMapBindIP(const bool hasIP, const uint32 parsedIP) -> uint32
{
    return hasIP ? parsedIP : 0;
}

// ResolveMapBindPort mirrors: present(--port) ? stoi(*maybe) : 0.
// hasPort is whether --port was present; parsedPort is host-evaluated stoi result
// (truncated/clamped by the host before inject if needed).
inline auto ResolveMapBindPort(const bool hasPort, const int parsedPort) -> uint16
{
    if (!hasPort)
    {
        return 0;
    }
    if (parsedPort < 0)
    {
        return 0;
    }
    if (parsedPort > 65535)
    {
        return 65535;
    }
    return static_cast<uint16>(parsedPort);
}

// BuildMapEngineConfigInputs assembles the pure config inputs tuple.
inline auto BuildMapEngineConfigInputs(
    const uint32 ip,
    const uint16 port,
    const bool   inCI,
    const bool   lazyZones,
    const bool   rebuildNavmeshes) -> MapEngineConfigInputs
{
    return MapEngineConfigInputs{
        .ip               = ip,
        .port             = port,
        .inCI             = inCI,
        .lazyZones        = lazyZones,
        .rebuildNavmeshes = rebuildNavmeshes,
    };
}

// MapMainExitCode mirrors main's success ? EXIT_SUCCESS : EXIT_FAILURE.
// EXIT_SUCCESS is 0; EXIT_FAILURE is typically 1.
// Go host pure half: mapapp.MapMain uses this polarity (slice 6375).
inline auto MapMainExitCode(const bool runSuccess) -> int
{
    return runSuccess ? 0 : 1;
}

// FormatMapFatalException mirrors ShowCriticalFmt("Fatal Exception: {}", e.what()).
inline auto FormatMapFatalException(const std::string& what) -> std::string
{
    return fmt::format("Fatal Exception: {}", what);
}

// ShouldTerminateOnNullEngine mirrors !engine_ after createEngine.
inline auto ShouldTerminateOnNullEngine(const bool engineCreated) -> bool
{
    return !engineCreated;
}

} // namespace mapapp
