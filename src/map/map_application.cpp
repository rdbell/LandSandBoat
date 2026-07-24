/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "map_application.h"

#include "common/arguments.h"
#include "common/console_service.h"
#include "common/utils.h"
#include "map_app_config.h"
#include "map_engine.h"
#include "map_networking.h"
#include "map_socket.h"

#ifdef _WIN32
#include <io.h>
#endif

namespace
{

auto appConfig() -> ApplicationConfig
{
    return ApplicationConfig{
        .serverName = mapapp::MapServerName,
        .arguments  = mapapp::BuildArgumentDefinitions(),
    };
}

} // namespace

MapApplication::MapApplication(const int argc, char** argv)
: Application(appConfig(), argc, argv)
{
    uint32 ip   = 0;
    uint16 port = 0;

    if (const auto maybeIP = args().present(std::string(mapapp::MapCLIArguments[0].name)))
    {
        ip = mapapp::ResolveMapBindIP(true, str2ip(*maybeIP));
    }
    else
    {
        ip = mapapp::ResolveMapBindIP(false, 0);
    }

    if (const auto maybePort = args().present(std::string(mapapp::MapCLIArguments[1].name)))
    {
        port = mapapp::ResolveMapBindPort(true, std::stoi(*maybePort));
    }
    else
    {
        port = mapapp::ResolveMapBindPort(false, 0);
    }

    const auto pure = mapapp::BuildMapEngineConfigInputs(
        ip,
        port,
        Application::isRunningInCI(),
        args().get<bool>(std::string(mapapp::MapCLIArguments[2].name)),
        args().get<bool>(std::string(mapapp::MapCLIArguments[3].name)));

    // Go host pure half: mapapp.BuildMapConfigFromInputs (slice 6397)
    // constructs MapConfig{ IPP(pure.ip, pure.port), flags }.
    engineConfig_.ipp              = IPP(pure.ip, pure.port);
    engineConfig_.inCI             = pure.inCI;
    engineConfig_.lazyZones        = pure.lazyZones;
    engineConfig_.rebuildNavmeshes = pure.rebuildNavmeshes;
}

MapApplication::~MapApplication()
{
}

auto MapApplication::createEngine() -> std::unique_ptr<Engine>
{
    // Go host pure half: mapapp.CreateMapEngine / CreateMapEngineFactory (6397).
    return std::make_unique<MapEngine>(*this, engineConfig_);
}

void MapApplication::registerCommands(ConsoleService& console)
{
    auto* mapEngine = static_cast<MapEngine*>(engine_.get());

    // Go host pure half: mapapp.RegisterMapEngineConsoleCommands (slice 6396).
    console.registerCommand(mapapp::MapGMCommandName, mapapp::MapGMCommandHelp, std::bind(&MapEngine::onGM, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapReloadRecipesCommandName, mapapp::MapReloadRecipesCommandHelp, std::bind(&MapEngine::onReloadRecipes, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapStatsCommandName, mapapp::MapStatsCommandHelp, std::bind(&MapEngine::onStats, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapBacktraceCommandName, mapapp::MapBacktraceCommandHelp, std::bind(&MapEngine::onBacktrace, mapEngine, std::placeholders::_1));
}

auto MapApplication::run() -> bool
{
    // Go host pure half: mapapp.MapApplicationRun (slice 6375) composes
    // null-engine terminate gate, init/registerCommands/postInit, and
    // FormatMapFatalException catch path.
    engine_ = createEngine();
    if (!engine_)
    {
        std::terminate();
    }

    engine_->onInitialize();
    registerCommands(console());

    scheduler_.postToMainThread(static_cast<MapEngine*>(engine_.get())->init());

    try
    {
        scheduler_.run(); // blocks
    }
    catch (const std::exception& e)
    {
        ShowCriticalFmt("{}", mapapp::FormatMapFatalException(e.what()));
        return false;
    }

    return true;
}
