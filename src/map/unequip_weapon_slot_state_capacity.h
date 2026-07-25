/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Team

  This file is part of LandSandBoat.

  LandSandBoat is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LandSandBoat is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LandSandBoat.  If not, see <http://www.gnu.org/licenses/>.

===========================================================================
*/

#pragma once

#include <cstdint>

namespace unequipweaponslotstatehelpers
{
constexpr uint8_t SlotSub    = 1;
constexpr uint8_t SlotRanged = 2;
constexpr uint8_t SlotAmmo   = 3;

enum class Action : uint8_t
{
    None,
    SetUnarmed,
    Clear,
};

struct Plan
{
    uint8_t slot{};
    Action  action{ Action::None };
};

constexpr Plan PlanFor(const uint8_t equipSlotID)
{
    switch (equipSlotID)
    {
        case SlotSub: return { .slot = SlotSub, .action = Action::SetUnarmed };
        case SlotAmmo: return { .slot = SlotAmmo, .action = Action::Clear };
        case SlotRanged: return { .slot = SlotRanged, .action = Action::Clear };
        default: return {};
    }
}
} // namespace unequipweaponslotstatehelpers
