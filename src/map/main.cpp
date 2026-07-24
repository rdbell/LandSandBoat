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

#include <map/map_application.h>
#include <map/map_app_config.h>

#include <common/lua.h>

#include <cstdlib>
#include <memory>

int main(int argc, char** argv)
{
    // Go host pure half: mapapp.NewMapApplication + MapMain (slice 6375)
    // drives Application.Run with success→0/failure→1 polarity and lua_cleanup
    // after explicit Application destroy.
    auto mapApp = std::make_unique<MapApplication>(argc, argv);

    const auto success = mapApp->run();

    const auto exitCode = mapapp::MapMainExitCode(success);

    // Explicitly destroy MapApplication before the lua state get cleaned up
    mapApp.reset();

    // TODO: This should be in ~Application but it needs more testing for xi_map
    // TODO: This wouldn't be needed if lua wasn't global
    lua_cleanup();

    return exitCode;
}
