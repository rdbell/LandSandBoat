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

namespace unequipsubstatehelpers
{
constexpr uint8_t SlotSub = 1;

struct Facts
{
    uint8_t equipSlotID{};
    bool    hasMainAfterClear{};
    bool    mainAfterClearIsEquipment{};
};

struct Plan
{
    bool checkUnarmedWeapon{};
    bool clearDualWield{};
};

constexpr Plan PlanFor(Facts facts)
{
    if (facts.equipSlotID != SlotSub)
    {
        return {};
    }

    return {
        .checkUnarmedWeapon = !facts.hasMainAfterClear || !facts.mainAfterClearIsEquipment,
        .clearDualWield     = true,
    };
}
} // namespace unequipsubstatehelpers
