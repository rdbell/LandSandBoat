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

#include <chrono>

#include "base.h"

enum class GP_CLI_COMMAND_CAMP_MODE : uint32_t
{
    Toggle = 0x00,
    On     = 0x01,
    Off    = 0x02,
};

namespace camp
{

// The state mutations selected by a CAMP packet. Keeping this decision
// independent of CCharEntity makes it possible to test the packet behavior
// without constructing map-server entities.
enum class HealingAction : uint8_t
{
    None,
    Add,
    Remove,
};

struct HealingTransition
{
    HealingAction       action{ HealingAction::None };
    bool                clearStateStack{};
    bool                disengageAutomatonPet{};
    std::chrono::seconds tick{};
};

// Selects the healing state mutation for a CAMP packet. Invalid modes are
// harmless no-ops, including values accepted by CAMP's legacy validation
// quirk but not handled by its process switch.
auto HealingTransitionFor(uint32_t mode, bool isHealing, bool hasAutomatonPet, uint8_t healingTickDelay) -> HealingTransition;

} // namespace camp

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00E8
// This packet is sent by the client when requesting to heal. (/heal)
GP_CLI_PACKET(GP_CLI_COMMAND_CAMP,
              uint32_t Mode; // PS2: Mode
);
