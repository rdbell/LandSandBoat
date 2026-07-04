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

#include "test_timer_helpers.h"

#include "common/timer.h"

#include <chrono>
#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "timer helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectDurationBetween(
    const timer::duration actual,
    const timer::duration min,
    const timer::duration max,
    const std::string& label) -> bool
{
    if (actual < min || actual > max)
    {
        std::cerr << "timer helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testCountMilliseconds() -> bool
{
    bool ok = true;

    ok = expectEqual(timer::count_milliseconds(0ns), int64{ 0 }, "milliseconds zero") && ok;
    ok = expectEqual(timer::count_milliseconds(999us), int64{ 0 }, "milliseconds sub-unit") && ok;
    ok = expectEqual(timer::count_milliseconds(1ms), int64{ 1 }, "milliseconds one") && ok;
    ok = expectEqual(timer::count_milliseconds(1999999ns), int64{ 1 }, "milliseconds floor") && ok;
    ok = expectEqual(timer::count_milliseconds(1500ms), int64{ 1500 }, "milliseconds large") && ok;
    ok = expectEqual(timer::count_milliseconds(-1ns), int64{ -1 }, "milliseconds negative tiny") && ok;
    ok = expectEqual(timer::count_milliseconds(-1500us), int64{ -2 }, "milliseconds negative floor") && ok;
    ok = expectEqual(timer::count_milliseconds(-1500ms), int64{ -1500 }, "milliseconds negative exact") && ok;

    return ok;
}

auto testCountSeconds() -> bool
{
    bool ok = true;

    ok = expectEqual(timer::count_seconds(0ns), int64{ 0 }, "seconds zero") && ok;
    ok = expectEqual(timer::count_seconds(999ms), int64{ 0 }, "seconds sub-unit") && ok;
    ok = expectEqual(timer::count_seconds(1s), int64{ 1 }, "seconds one") && ok;
    ok = expectEqual(timer::count_seconds(1999ms), int64{ 1 }, "seconds floor") && ok;
    ok = expectEqual(timer::count_seconds(1500s), int64{ 1500 }, "seconds large") && ok;
    ok = expectEqual(timer::count_seconds(-1ns), int64{ -1 }, "seconds negative tiny") && ok;
    ok = expectEqual(timer::count_seconds(-1500ms), int64{ -2 }, "seconds negative floor") && ok;
    ok = expectEqual(timer::count_seconds(-1500s), int64{ -1500 }, "seconds negative exact") && ok;

    return ok;
}

auto testOffsetAndUptime() -> bool
{
    bool ok = true;

    timer::reset_offset();

    const auto before = timer::now();
    timer::add_offset(2s);
    const auto adjusted = timer::now();
    ok                  = expectDurationBetween(adjusted - before, 1900ms, 2500ms, "now offset") && ok;

    const auto uptimeBefore = timer::get_uptime();
    timer::add_offset(10s);
    const auto uptimeAfter = timer::get_uptime();
    ok                     = expectDurationBetween(uptimeAfter - uptimeBefore, 0s, 5s, "uptime ignores offset") && ok;

    timer::reset_offset();
    const auto reset = timer::now();
    ok               = expectDurationBetween(reset - adjusted, -2500ms, -1500ms, "reset offset") && ok;

    timer::reset_offset();

    return ok;
}

} // namespace

auto runTimerHelpersSelfTests() -> bool
{
    bool ok = true;

    ok = testCountMilliseconds() && ok;
    ok = testCountSeconds() && ok;
    ok = testOffsetAndUptime() && ok;

    return ok;
}
