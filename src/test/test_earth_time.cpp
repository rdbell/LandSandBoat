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

#include "test_earth_time.h"

#include "common/earth_time.h"

#include <chrono>
#include <iostream>
#include <string>

using namespace std::chrono_literals;

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "earth_time self-test failed: " << label << " expected " << expected << ", got " << actual << '\n';
        return false;
    }

    return true;
}

auto expectUtcFields(const earth_time::time_point tp,
                     const std::string&           label,
                     const int32                   year,
                     const uint32                  month,
                     const uint32                  monthday,
                     const uint32                  yearday,
                     const uint32                  weekday,
                     const uint32                  hour,
                     const uint32                  minute,
                     const uint32                  second) -> bool
{
    bool ok = true;
    ok      = expectEqual(earth_time::utc::get_year(tp), year, label + " utc year") && ok;
    ok      = expectEqual(earth_time::utc::get_month(tp), month, label + " utc month") && ok;
    ok      = expectEqual(earth_time::utc::get_monthday(tp), monthday, label + " utc monthday") && ok;
    ok      = expectEqual(earth_time::utc::get_yearday(tp), yearday, label + " utc yearday") && ok;
    ok      = expectEqual(earth_time::utc::get_weekday(tp), weekday, label + " utc weekday") && ok;
    ok      = expectEqual(earth_time::utc::get_hour(tp), hour, label + " utc hour") && ok;
    ok      = expectEqual(earth_time::utc::get_minute(tp), minute, label + " utc minute") && ok;
    ok      = expectEqual(earth_time::utc::get_second(tp), second, label + " utc second") && ok;
    return ok;
}

auto expectJstFields(const earth_time::time_point tp,
                     const std::string&           label,
                     const int32                   year,
                     const uint32                  month,
                     const uint32                  monthday,
                     const uint32                  yearday,
                     const uint32                  weekday,
                     const uint32                  hour,
                     const uint32                  minute,
                     const uint32                  second) -> bool
{
    bool ok = true;
    ok      = expectEqual(earth_time::jst::get_year(tp), year, label + " jst year") && ok;
    ok      = expectEqual(earth_time::jst::get_month(tp), month, label + " jst month") && ok;
    ok      = expectEqual(earth_time::jst::get_monthday(tp), monthday, label + " jst monthday") && ok;
    ok      = expectEqual(earth_time::jst::get_yearday(tp), yearday, label + " jst yearday") && ok;
    ok      = expectEqual(earth_time::jst::get_weekday(tp), weekday, label + " jst weekday") && ok;
    ok      = expectEqual(earth_time::jst::get_hour(tp), hour, label + " jst hour") && ok;
    ok      = expectEqual(earth_time::jst::get_minute(tp), minute, label + " jst minute") && ok;
    ok      = expectEqual(earth_time::jst::get_second(tp), second, label + " jst second") && ok;
    return ok;
}

} // namespace

auto runEarthTimeSelfTests() -> bool
{
    bool ok = true;

    const earth_time::time_point friday{ 1783036800s };
    ok = expectEqual(earth_time::timestamp(friday), 1783036800U, "friday timestamp") && ok;
    ok = expectUtcFields(friday, "friday", 2026, 7, 3, 183, 5, 0, 0, 0) && ok;
    ok = expectJstFields(friday, "friday", 2026, 7, 3, 183, 5, 9, 0, 0) && ok;
    ok = expectEqual(earth_time::timestamp(earth_time::utc::get_next_midnight(friday)), 1783036800U, "friday utc midnight") && ok;
    ok = expectEqual(earth_time::timestamp(earth_time::jst::get_next_midnight(friday)), 1783090800U, "friday jst midnight") && ok;
    ok = expectEqual(static_cast<uint32>(earth_time::get_game_weekday(friday)), 4U, "friday game weekday") && ok;
    ok = expectEqual(earth_time::timestamp(earth_time::get_next_game_week(friday)), 1783263600U, "friday next game week") && ok;

    const earth_time::time_point sundayBeforeReset{ 1783263599s };
    ok = expectUtcFields(sundayBeforeReset, "sunday before reset", 2026, 7, 5, 186, 0, 14, 59, 59) && ok;
    ok = expectJstFields(sundayBeforeReset, "sunday before reset", 2026, 7, 5, 186, 0, 23, 59, 59) && ok;
    ok = expectEqual(earth_time::timestamp(earth_time::get_next_game_week(sundayBeforeReset)), 1783263600U, "sunday before reset next game week") && ok;

    const earth_time::time_point mondayAtReset{ 1783263600s };
    ok = expectJstFields(mondayAtReset, "monday at reset", 2026, 7, 6, 186, 1, 0, 0, 0) && ok;
    ok = expectEqual(earth_time::timestamp(earth_time::get_next_game_week(mondayAtReset)), 1783782000U, "monday at reset next game week") && ok;

    const earth_time::time_point justAfterUtcMidnight{ 1783036800s + 1ns };
    ok = expectEqual(earth_time::timestamp(earth_time::utc::get_next_midnight(justAfterUtcMidnight)), 1783123200U, "utc just after midnight") && ok;

    const earth_time::time_point justAfterJstMidnight{ 1783263600s + 1ns };
    ok = expectEqual(earth_time::timestamp(earth_time::jst::get_next_midnight(justAfterJstMidnight)), 1783350000U, "jst just after midnight") && ok;

    const earth_time::time_point preEpoch{ -500ms };
    ok = expectEqual(earth_time::timestamp(preEpoch), 4294967295U, "pre epoch timestamp wraps") && ok;

    return ok;
}
