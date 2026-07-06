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

#include "test_rate_limit_utils.h"

#include "common/utils.h"

#include <chrono>
#include <iostream>
#include <string>

namespace
{
using namespace std::chrono_literals;

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "rate limit utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto resetRateLimitState() -> void
{
    lastExecutionTimes.write([](auto& values) {
        values.clear();
        return 0;
    });
    timer::reset_offset();
}

auto trackedEntryCount() -> std::size_t
{
    return lastExecutionTimes.read([](const auto& values) {
        return values.size();
    });
}

auto onlyTrackedKey() -> std::string
{
    return lastExecutionTimes.read([](const auto& values) {
        if (values.empty())
        {
            return std::string{};
        }

        return values.begin()->first;
    });
}

auto trackedTime(const std::string& key) -> timer::time_point
{
    return lastExecutionTimes.read([&](const auto& values) {
        return values.at(key);
    });
}

auto setTrackedTime(const std::string& key, const timer::time_point value) -> void
{
    lastExecutionTimes.write([&](auto& values) {
        values[key] = value;
        return 0;
    });
}

auto firstRateLimitedLocation(int& hits) -> void
{
    RATE_LIMIT(5s, ++hits);
}

auto secondRateLimitedLocation(int& hits) -> void
{
    RATE_LIMIT(5s, ++hits);
}

auto boundaryRateLimitedLocation(int& hits) -> void
{
    RATE_LIMIT(5s, ++hits);
}

auto testFirstCallAndRepeatedCallAtSameLocation() -> bool
{
    resetRateLimitState();

    bool ok = true;
    int  hits = 0;

    firstRateLimitedLocation(hits);
    ok = expectEqual(hits, 1, "first call executes") && ok;
    ok = expectEqual(trackedEntryCount(), std::size_t{ 1 }, "first call records one key") && ok;

    const auto key       = onlyTrackedKey();
    const auto timestamp = trackedTime(key);

    firstRateLimitedLocation(hits);
    ok = expectEqual(hits, 1, "same source location is suppressed") && ok;
    ok = expectEqual(trackedEntryCount(), std::size_t{ 1 }, "suppressed call keeps one key") && ok;
    ok = expectEqual(trackedTime(key), timestamp, "suppressed call keeps timestamp") && ok;

    resetRateLimitState();
    return ok;
}

auto testDifferentSourceLocationsTrackIndependently() -> bool
{
    resetRateLimitState();

    bool ok = true;
    int  hits = 0;

    firstRateLimitedLocation(hits);
    secondRateLimitedLocation(hits);

    ok = expectEqual(hits, 2, "different source locations both execute") && ok;
    ok = expectEqual(trackedEntryCount(), std::size_t{ 2 }, "different source locations record separate keys") && ok;

    resetRateLimitState();
    return ok;
}

auto testCooldownWindowAndTimestampUpdate() -> bool
{
    resetRateLimitState();

    bool ok = true;
    int  hits = 0;

    boundaryRateLimitedLocation(hits);
    ok = expectEqual(hits, 1, "boundary seed call executes") && ok;

    const auto key = onlyTrackedKey();

    const auto beforeBoundary = timer::now() - 5s + 250ms;
    setTrackedTime(key, beforeBoundary);
    boundaryRateLimitedLocation(hits);
    ok = expectEqual(hits, 1, "call before cooldown boundary is suppressed") && ok;
    ok = expectEqual(trackedTime(key), beforeBoundary, "suppressed before-boundary timestamp unchanged") && ok;

    const auto afterBoundary = timer::now() - 5s - 250ms;
    setTrackedTime(key, afterBoundary);
    boundaryRateLimitedLocation(hits);
    ok = expectEqual(hits, 2, "call after cooldown boundary executes") && ok;
    ok = expectEqual(trackedTime(key) > afterBoundary, true, "allowed call updates timestamp") && ok;

    resetRateLimitState();
    return ok;
}

} // namespace

auto runRateLimitUtilsSelfTests() -> bool
{
    bool ok = true;

    ok = testFirstCallAndRepeatedCallAtSameLocation() && ok;
    ok = testDifferentSourceLocationsTrackIndependently() && ok;
    ok = testCooldownWindowAndTimestampUpdate() && ok;

    return ok;
}
