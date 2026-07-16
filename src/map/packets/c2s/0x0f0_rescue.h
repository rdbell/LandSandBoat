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

namespace rescue
{

// The externally visible actions selected by a self-unstuck request. Keeping
// this decision separate from CCharEntity makes the cooldown behavior testable
// without map-server state.
enum class Action : uint8_t
{
    None,
    SendCooldownMessage,
    WarpHomePoint,
};

struct Transition
{
    Action   action{ Action::None };
    bool     setCooldown{};
    uint32_t cooldownExpiry{};
};

// Selects the self-unstuck mutation. A disabled feature or active cooldown is
// non-mutating; successful warps set the character-variable expiry to now plus
// the configured cooldown (with the uint32 timestamp wrap behavior used by
// the live handler).
auto TransitionFor(bool selfUnstuckEnabled, bool cooldownActive, uint32_t now, uint32_t cooldown) -> Transition;

} // namespace rescue

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x00F0
// This packet is sent by the client when requesting to be unstuck via the GM Help Desk system.
GP_CLI_PACKET(GP_CLI_COMMAND_RESCUE,
              uint32_t State; // PS2: State
);
