/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

namespace submap
{

// The persistence work requested after accepting SUBMAPCHANGE. It is kept
// separate from CCharEntity so packet behavior can be tested without a map
// server entity.
struct ChangeTransition
{
    uint16_t boundary{};
    bool     savePosition{};
};

// The reply requested after accepting REQSUBMAPNUM.
struct NumberReplyPlan
{
    bool     sendReply{};
    uint32_t mapNumber{};
};

// Selects SUBMAPCHANGE's boundary update and persistence request.
auto ChangeTransitionFor(uint16_t subMapNumber) -> ChangeTransition;

// Selects REQSUBMAPNUM's reply. The legacy packet replies with map number
// zero only while an NPC lock is active.
auto NumberReplyPlanFor(bool isNpcLocked) -> NumberReplyPlan;

} // namespace submap
