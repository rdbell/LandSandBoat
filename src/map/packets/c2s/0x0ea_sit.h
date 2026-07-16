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

#include <cstdint>

#include "base.h"

enum class GP_CLI_COMMAND_SIT_MODE : uint32_t
{
    Toggle = 0x00,
    On     = 0x01,
    Off    = 0x02,
};

namespace sit
{

// PetKind is the subset of pet identity relevant to /sit animation mirroring.
enum class PetKind : uint8_t
{
    None,
    Other,
    WYVERN,
    AUTOMATON,
};

// Transition is the entity-independent state change requested by /sit.
struct Transition
{
    bool    removeHealingSilently = true;
    uint8_t characterAnimation    = 0;
    bool    updateCharacterHP     = true;
    bool    updatePet             = false;
};

// TransitionFor mirrors GP_CLI_COMMAND_SIT::process without entity ownership.
// Unknown modes preserve the current animation, matching the packet handler's
// switch behavior when called without prior validation.
auto TransitionFor(uint32_t mode, uint8_t currentAnimation, PetKind petKind) -> Transition;

} // namespace sit

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00EA
// This packet is sent by the client when requesting to sit. (/sit)
GP_CLI_PACKET(GP_CLI_COMMAND_SIT,
              uint32_t Mode; // PS2: Mode
);
