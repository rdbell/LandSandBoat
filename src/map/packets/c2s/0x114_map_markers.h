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

#pragma once
#include "base.h"

// Captures MAP_MARKERS' fixed post-validation response without coupling tests
// to the map session or packet transport.
namespace mapmarkershelpers
{
enum class Action : uint8
{
    SendHomepoints,
};

constexpr auto SelectAction() -> Action
{
    return Action::SendHomepoints;
}
} // namespace mapmarkershelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0114
// This packet is sent by the client when requesting the current maps markers. (ie. Homepoint Crystals, Abyssea Maws, Survival Guides, Waypoints, Telepoints, etc.)
GP_CLI_PACKET(GP_CLI_COMMAND_MAP_MARKERS);
