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

#include "test_c2s_submap_transition.h"

#include <cstdint>
#include <iostream>
#include <string>

#include "map/packets/c2s/submap_transition.h"

namespace
{

auto expectEqualUint(const uint64_t actual, const uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s submap transition self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualBool(const bool actual, const bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "c2s submap transition self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testSubmapChangeTransitions() -> bool
{
    bool ok = true;
    for (const auto subMapNumber : { uint16_t{ 0 }, uint16_t{ 0x3344 }, uint16_t{ 0xFFFF } })
    {
        const auto transition = submap::ChangeTransitionFor(subMapNumber);
        ok                    = expectEqualUint(transition.boundary, subMapNumber, "SUBMAPCHANGE boundary") && ok;
        ok                    = expectEqualBool(transition.savePosition, true, "SUBMAPCHANGE saves position") && ok;
    }
    return ok;
}

auto testReqSubmapNumReplyPlans() -> bool
{
    bool ok = true;

    const auto unlocked = submap::NumberReplyPlanFor(false);
    ok                  = expectEqualBool(unlocked.sendReply, false, "REQSUBMAPNUM unlocked no reply") && ok;

    const auto npcLocked = submap::NumberReplyPlanFor(true);
    ok                   = expectEqualBool(npcLocked.sendReply, true, "REQSUBMAPNUM npc locked reply") && ok;
    ok                   = expectEqualUint(npcLocked.mapNumber, 0, "REQSUBMAPNUM reply map number") && ok;
    return ok;
}

} // namespace

auto runC2SSubmapTransitionSelfTests() -> bool
{
    return testSubmapChangeTransitions() &&
           testReqSubmapNumReplyPlans();
}
