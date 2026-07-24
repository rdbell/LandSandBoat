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

#include "search_application.h"

#include "common/console_service.h"
#include "search_application_config.h"
#include "search_engine.h"

SearchApplication::SearchApplication(const int argc, char** argv)
: Application(SearchApplicationConfig(), argc, argv)
{
}

SearchApplication::~SearchApplication() = default;

auto SearchApplication::createEngine() -> std::unique_ptr<Engine>
{
    return std::make_unique<SearchEngine>(scheduler_);
}

void SearchApplication::registerCommands(ConsoleService& console)
{
    // Go host pure half: searchutil.PlanSearchRegisterCommands (slice 6374).
    const auto expiryDays    = settings::get<uint16>("search.EXPIRE_DAYS");
    const auto commands      = SearchApplicationConsoleCommandDescriptors(expiryDays);
    auto*      searchEngine  = static_cast<SearchEngine*>(engine_.get());
    const auto& ahCleanup    = commands[0];
    const auto& expireAll    = commands[1];

    console.registerCommand(ahCleanup.name,
                            ahCleanup.description,
                            std::bind(&SearchEngine::onAHCleanup, searchEngine, std::placeholders::_1));

    console.registerCommand(expireAll.name,
                            expireAll.description,
                            std::bind(&SearchEngine::onExpireAll, searchEngine, std::placeholders::_1));
}
