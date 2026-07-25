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

namespace unequipweaponfinalizehelpers
{
constexpr uint8_t SlotMain   = 0;
constexpr uint8_t SlotSub    = 1;
constexpr uint8_t SlotRanged = 2;

struct Facts
{
    uint8_t equipSlotID{};
    bool    removedRangedIsStringInstrument{};
    bool    removedRangedIsWindInstrument{};
};

struct Plan
{
    bool clearTP{};
    bool clearAftermath{};
    bool buildWeaponSkills{};
};

constexpr Plan PlanFor(Facts facts)
{
    if (facts.equipSlotID == SlotSub || facts.equipSlotID == SlotMain)
    {
        return {
            .clearTP           = true,
            .clearAftermath    = true,
            .buildWeaponSkills = true,
        };
    }

    if (facts.equipSlotID == SlotRanged)
    {
        const bool isInstrument = facts.removedRangedIsStringInstrument || facts.removedRangedIsWindInstrument;
        return {
            .clearTP           = !isInstrument,
            .clearAftermath    = !isInstrument,
            .buildWeaponSkills = true,
        };
    }

    return {};
}
} // namespace unequipweaponfinalizehelpers
