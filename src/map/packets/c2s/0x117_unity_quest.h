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

// UNITY_QUEST has no payload-dependent process branch: after validation
// accepts the request, it refreshes both the UNITY packet and Unity packet
// collection for the requesting character.
namespace unityquesthelpers
{

enum class Action : uint8
{
    SendUnityAndUnityPackets,
};

[[nodiscard]] constexpr auto SelectAction() -> Action
{
    return Action::SendUnityAndUnityPackets;
}

} // namespace unityquesthelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x0117
// This packet is sent by the client when requesting its Unity quest information.
GP_CLI_PACKET(GP_CLI_COMMAND_UNITY_QUEST,
              uint32_t Kind; // Always 0x0
);
