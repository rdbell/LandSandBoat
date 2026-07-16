/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_world_time_server_effects.h"

#include "world/time_server.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{

class RecordingEffects final : public TimeServerTickEffects
{
public:
    void updateWeeklyConquest() override
    {
        calls.emplace_back("weekly");
    }

    void updateHourlyConquest() override
    {
        calls.emplace_back("hourly");
    }

    void updateDailyTally() override
    {
        calls.emplace_back("daily-tally");
    }

    void updateVanaHourlyConquest() override
    {
        calls.emplace_back("vana-hourly");
    }

    std::vector<std::string> calls;
};

auto expectCalls(const TimeServerTickInput& input, const std::vector<std::string>& expected, const std::string& label) -> bool
{
    RecordingEffects effects;
    dispatchTimeServerTickEffects(input, effects);

    if (effects.calls == expected)
    {
        return true;
    }

    std::cerr << "World time-server effects self-test failed: " << label << '\n';
    return false;
}

auto testNoOp() -> bool
{
    return expectCalls({}, {}, "no hourly boundaries");
}

auto testEarthEffects() -> bool
{
    bool ok = true;
    ok      = expectCalls({ .earthHourlyTick = true, .jstHour = 0, .jstWeekday = 1 }, { "weekly", "daily-tally" }, "Monday midnight") && ok;
    ok      = expectCalls({ .earthHourlyTick = true, .jstHour = 0, .jstWeekday = 2 }, { "hourly", "daily-tally" }, "non-Monday midnight") && ok;
    ok      = expectCalls({ .earthHourlyTick = true, .jstHour = 13, .jstWeekday = 1 }, { "hourly" }, "ordinary earth hour") && ok;
    return ok;
}

auto testVanaEffectsAndOrdering() -> bool
{
    bool ok = true;
    ok      = expectCalls({ .vanaHourlyTick = true }, { "vana-hourly" }, "Vana hour only") && ok;
    ok      = expectCalls({ .earthHourlyTick = true, .jstHour = 0, .jstWeekday = 1, .vanaHourlyTick = true }, { "weekly", "daily-tally", "vana-hourly" }, "earth before Vana") && ok;
    return ok;
}

} // namespace

auto runWorldTimeServerEffectsSelfTests() -> bool
{
    return testNoOp() && testEarthEffects() && testVanaEffectsAndOrdering();
}
