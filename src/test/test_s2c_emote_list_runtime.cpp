/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_emote_list_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x11a_emote_list.h"

namespace
{

auto expectPlan(const emotelisthelpers::Plan actual, const uint32 expectedJobEmotes, const uint16 expectedChairs, const char* label) -> bool
{
    if (actual.jobEmotes != expectedJobEmotes || actual.chairs != expectedChairs)
    {
        std::cerr << "s2c EMOTE_LIST runtime self-test failed: " << label << " got " << actual.jobEmotes << '/' << actual.chairs
                  << " expected " << expectedJobEmotes << '/' << expectedChairs << '\n';
        return false;
    }
    return true;
}

auto testNoOwnership() -> bool
{
    return expectPlan(emotelisthelpers::PlanFor({}), 0, 0, "no key items leaves all defined and unused bits zero");
}

auto testSparseOwnership() -> bool
{
    emotelisthelpers::Ownership ownership{};
    ownership.jobEmotes[0]  = true; // WAR
    ownership.jobEmotes[10] = true; // RNG
    ownership.jobEmotes[21] = true; // RUN
    ownership.chairs[0]     = true; // Chair1
    ownership.chairs[5]     = true; // Chair6
    ownership.chairs[10]    = true; // Chair11
    return expectPlan(emotelisthelpers::PlanFor(ownership), 0x00200401, 0x0421, "first middle and last key items map to their bits");
}

auto testFullOwnership() -> bool
{
    emotelisthelpers::Ownership ownership{};
    ownership.jobEmotes.fill(true);
    ownership.chairs.fill(true);
    return expectPlan(emotelisthelpers::PlanFor(ownership), 0x003FFFFF, 0x07FF, "all key items leave unused bits zero");
}

} // namespace

auto runS2CEmoteListRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok      = testNoOwnership() && ok;
    ok      = testSparseOwnership() && ok;
    ok      = testFullOwnership() && ok;
    return ok;
}
