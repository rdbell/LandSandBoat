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
    auto ip   = 0;
    auto port = 0;

    if (const auto maybeIP = args().present(std::string(mapapp::MapCLIArguments[0].name)))
    {
        ip = str2ip(*maybeIP);
    }

    if (const auto maybePort = args().present(std::string(mapapp::MapCLIArguments[1].name)))
    {
        port = std::stoi(*maybePort);
    }

    engineConfig_.ipp              = IPP(ip, port);
    engineConfig_.inCI             = Application::isRunningInCI();
    engineConfig_.lazyZones        = args().get<bool>(std::string(mapapp::MapCLIArguments[2].name));
    engineConfig_.rebuildNavmeshes = args().get<bool>(std::string(mapapp::MapCLIArguments[3].name));
}

MapApplication::~MapApplication()
{
}

auto MapApplication::createEngine() -> std::unique_ptr<Engine>
{
    return std::make_unique<MapEngine>(*this, engineConfig_);
}

void MapApplication::registerCommands(ConsoleService& console)
{
    auto* mapEngine = static_cast<MapEngine*>(engine_.get());

    console.registerCommand(mapapp::MapGMCommandName, mapapp::MapGMCommandHelp, std::bind(&MapEngine::onGM, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapReloadRecipesCommandName, mapapp::MapReloadRecipesCommandHelp, std::bind(&MapEngine::onReloadRecipes, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapStatsCommandName, mapapp::MapStatsCommandHelp, std::bind(&MapEngine::onStats, mapEngine, std::placeholders::_1));
    console.registerCommand(mapapp::MapBacktraceCommandName, mapapp::MapBacktraceCommandHelp, std::bind(&MapEngine::onBacktrace, mapEngine, std::placeholders::_1));
}

auto MapApplication::run() -> bool
{
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
        ShowCriticalFmt("Fatal Exception: {}", e.what());
        return false;
    }

    return true;
}
