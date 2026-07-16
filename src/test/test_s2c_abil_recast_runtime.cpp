/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_abil_recast_runtime.h"

#include <chrono>
#include <iostream>

#include "map/packets/s2c/0x119_abil_recast.h"

namespace
{

auto expectEqual(const uint32 actual, const uint32 expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "s2c ABIL_RECAST runtime self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto apply(abilrecasthelpers::Plan& plan, uint8& count, const abilrecasthelpers::RecastFact& fact, const timer::time_point now) -> bool
{
    return abilrecasthelpers::Apply(plan, count, fact, now);
}

auto testTimeAndPlacement() -> bool
{
    using namespace std::chrono_literals;

    const auto              now = timer::time_point{} + 100s;
    abilrecasthelpers::Plan plan{};
    uint8                   count = 1;
    bool                    ok    = true;

    ok = !apply(plan, count, { Recast::Special, now, 1500ms }, now) && ok;
    ok = !apply(plan, count, { Recast::Mount, now, 2500ms }, now) && ok;
    ok = !apply(plan, count, { static_cast<Recast>(7), now, 1500ms }, now) && ok;
    ok = !apply(plan, count, { static_cast<Recast>(8), now - 5s, 1s }, now) && ok;
    ok = !apply(plan, count, { static_cast<Recast>(9), now + 5s, 0s }, now) && ok;

    ok = expectEqual(plan.timers[0].Timer, 2, "special rounds up into timer zero") && ok;
    ok = expectEqual(plan.timers[0].TimerId, 0, "special timer id") && ok;
    ok = expectEqual(plan.mountRecast, 3, "mount rounds up into packet tail") && ok;
    ok = expectEqual(plan.mountRecastId, static_cast<uint16>(Recast::Mount), "mount id into packet tail") && ok;
    ok = expectEqual(plan.timers[1].Timer, 2, "normal starts at timer one") && ok;
    ok = expectEqual(plan.timers[1].TimerId, 7, "normal timer one id") && ok;
    ok = expectEqual(plan.timers[2].Timer, 0, "expired recast clamps to zero") && ok;
    ok = expectEqual(plan.timers[3].Timer, 0, "zero recast stays zero") && ok;
    ok = expectEqual(count, 4, "mount and special do not consume normal timer slots") && ok;
    return ok;
}

auto testChargeCalculation() -> bool
{
    using namespace std::chrono_literals;

    const auto              now = timer::time_point{};
    abilrecasthelpers::Plan plan{};
    uint8                   count = 1;
    bool                    ok    = true;

    ok = !apply(plan, count, { static_cast<Recast>(10), now, 1s, 5s, 2, true, 10s }, now) && ok;
    ok = !apply(plan, count, { static_cast<Recast>(11), now, 1s, 10s, 2, true, 5s }, now) && ok;
    ok = !apply(plan, count, { static_cast<Recast>(12), now, 1s, 5s, 2, false, 10s }, now) && ok;

    ok = expectEqual(plan.timers[1].Calc2, 65526, "charge Calc2 only uses positive base difference") && ok;
    ok = expectEqual(plan.timers[2].Calc2, 0, "smaller base charge leaves Calc2 zero") && ok;
    ok = expectEqual(plan.timers[3].Calc2, 0, "missing base charge leaves Calc2 zero") && ok;
    return ok;
}

auto testNormalCounterCutoffAndOrdering() -> bool
{
    using namespace std::chrono_literals;

    const auto              now = timer::time_point{};
    abilrecasthelpers::Plan plan{};
    uint8                   count = 1;
    bool                    stopped{};

    for (uint16 id = 1; id <= 31; ++id)
    {
        stopped = apply(plan, count, { static_cast<Recast>(id), now, 1s }, now);
        if (stopped)
        {
            break;
        }
    }

    bool ok = true;
    ok      = expectEqual(stopped, 1, "thirtieth normal recast stops packet construction") && ok;
    ok      = expectEqual(count, 31, "normal counter ends after slot thirty") && ok;
    ok      = expectEqual(plan.timers[1].TimerId, 1, "first normal recast preserves order") && ok;
    ok      = expectEqual(plan.timers[30].TimerId, 30, "slot thirty receives thirtieth normal recast") && ok;
    return ok;
}

} // namespace

auto runS2CAbilRecastRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok      = testTimeAndPlacement() && ok;
    ok      = testChargeCalculation() && ok;
    ok      = testNormalCounterCutoffAndOrdering() && ok;
    return ok;
}
