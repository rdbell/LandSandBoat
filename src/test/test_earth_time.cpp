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
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>

using namespace std::chrono_literals;

namespace
{

class ScopedTimezone
{
public:
    explicit ScopedTimezone(const char* timezone)
    {
        if (const auto* previous = std::getenv("TZ"))
        {
            previous_ = previous;
        }

        set(timezone);
    }

    ~ScopedTimezone()
    {
        if (previous_)
        {
            set(previous_->c_str());
        }
        else
        {
#ifdef _WIN32
            _putenv_s("TZ", "");
            _tzset();
#else
            unsetenv("TZ");
            tzset();
#endif
        }
    }

    ScopedTimezone(const ScopedTimezone&)            = delete;
    ScopedTimezone& operator=(const ScopedTimezone&) = delete;

private:
    static void set(const char* timezone)
    {
#ifdef _WIN32
        _putenv_s("TZ", timezone);
        _tzset();
#else
        setenv("TZ", timezone, 1);
        tzset();
#endif
    }

    std::optional<std::string> previous_;
};

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

auto expectTmFields(const std::tm&      value,
                    const std::string&  label,
                    const int32         year,
                    const uint32        month,
                    const uint32        monthday,
                    const uint32        yearday,
                    const uint32        weekday,
                    const uint32        hour,
                    const uint32        minute,
                    const uint32        second,
                    const bool          isDst) -> bool
{
    bool ok = true;
    ok      = expectEqual(value.tm_year + 1900, year, label + " year") && ok;
    ok      = expectEqual(value.tm_mon + 1, month, label + " month") && ok;
    ok      = expectEqual(value.tm_mday, monthday, label + " monthday") && ok;
    ok      = expectEqual(value.tm_yday, yearday, label + " yearday") && ok;
    ok      = expectEqual(value.tm_wday, weekday, label + " weekday") && ok;
    ok      = expectEqual(value.tm_hour, hour, label + " hour") && ok;
    ok      = expectEqual(value.tm_min, minute, label + " minute") && ok;
    ok      = expectEqual(value.tm_sec, second, label + " second") && ok;
    ok      = expectEqual(value.tm_isdst > 0, isDst, label + " dst") && ok;
    return ok;
}

auto expectLocalFields(const earth_time::time_point tp,
                       const std::string&           label,
                       const int32                   year,
                       const uint32                  month,
                       const uint32                  monthday,
                       const uint32                  yearday,
                       const uint32                  weekday,
                       const uint32                  hour,
                       const uint32                  minute,
                       const uint32                  second,
                       const bool                    isDst) -> bool
{
    bool ok = true;
    ok      = expectEqual(earth_time::local::get_year(tp), year, label + " local year") && ok;
    ok      = expectEqual(earth_time::local::get_month(tp), month, label + " local month") && ok;
    ok      = expectEqual(earth_time::local::get_monthday(tp), monthday, label + " local monthday") && ok;
    ok      = expectEqual(earth_time::local::get_yearday(tp), yearday, label + " local yearday") && ok;
    ok      = expectEqual(earth_time::local::get_weekday(tp), weekday, label + " local weekday") && ok;
    ok      = expectEqual(earth_time::local::get_hour(tp), hour, label + " local hour") && ok;
    ok      = expectEqual(earth_time::local::get_minute(tp), minute, label + " local minute") && ok;
    ok      = expectEqual(earth_time::local::get_second(tp), second, label + " local second") && ok;
    ok      = expectEqual(earth_time::local::is_dst(tp), isDst, label + " local dst") && ok;
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

    const earth_time::time_point justAfterUtcMidnight{ 1783036800s + 1us };
    ok = expectEqual(earth_time::timestamp(earth_time::utc::get_next_midnight(justAfterUtcMidnight)), 1783123200U, "utc just after midnight") && ok;

    const earth_time::time_point justAfterJstMidnight{ 1783263600s + 1us };
    ok = expectEqual(earth_time::timestamp(earth_time::jst::get_next_midnight(justAfterJstMidnight)), 1783350000U, "jst just after midnight") && ok;

    const earth_time::time_point preEpoch{ -500ms };
    ok = expectEqual(earth_time::timestamp(preEpoch), 4294967295U, "pre epoch timestamp wraps") && ok;

    // system_clock::to_time_t truncates fractional seconds toward zero on the
    // supported standard libraries. That differs deliberately from timestamp,
    // which floors before converting to uint32.
    ok = expectTmFields(earth_time::to_utc_tm(earth_time::time_point{ 0s }), "unix epoch tm", 1970, 1, 1, 0, 4, 0, 0, 0, false) && ok;
    ok = expectTmFields(earth_time::to_utc_tm(preEpoch), "half second before epoch tm", 1970, 1, 1, 0, 4, 0, 0, 0, false) && ok;
    ok = expectTmFields(earth_time::to_utc_tm(earth_time::time_point{ -1500ms }), "one and a half seconds before epoch tm", 1969, 12, 31, 364, 3, 23, 59, 59, false) && ok;

    {
        // Keep the fixture independent of the host's timezone database while
        // exercising standard/daylight offsets. Microsoft _tzset accepts the
        // compact form and supplies the fixed US transition rules itself;
        // POSIX platforms accept explicit transition rules.
#ifdef _WIN32
        const ScopedTimezone timezone("EST5EDT");
#else
        const ScopedTimezone timezone("EST5EDT,M3.2.0/2,M11.1.0/2");
#endif
        const earth_time::time_point winter{ 1705322096s }; // 2024-01-15 12:34:56 UTC
        const earth_time::time_point summer{ 1721046896s }; // 2024-07-15 12:34:56 UTC

        ok = expectTmFields(earth_time::to_local_tm(winter), "winter local tm", 2024, 1, 15, 14, 1, 7, 34, 56, false) && ok;
        ok = expectLocalFields(winter, "winter", 2024, 1, 15, 14, 1, 7, 34, 56, false) && ok;
        ok = expectTmFields(earth_time::to_local_tm(summer), "summer local tm", 2024, 7, 15, 196, 1, 8, 34, 56, true) && ok;
        ok = expectLocalFields(summer, "summer", 2024, 7, 15, 196, 1, 8, 34, 56, true) && ok;
    }

    earth_time::reset_offset();
    ok = expectEqual(std::chrono::duration_cast<std::chrono::milliseconds>(earth_time::time_offset).count(), 0LL, "initial reset offset") && ok;
    earth_time::add_offset(2s);
    earth_time::add_offset(-2500ms);
    ok = expectEqual(std::chrono::duration_cast<std::chrono::milliseconds>(earth_time::time_offset).count(), -500LL, "accumulated signed offset") && ok;

    const auto wallBefore  = earth_time::clock::now();
    const auto adjustedNow = earth_time::now();
    const auto wallAfter   = earth_time::clock::now();
    ok = expectEqual(adjustedNow >= wallBefore - 500ms && adjustedNow <= wallAfter - 500ms, true, "now applies offset once") && ok;

    earth_time::reset_offset();
    ok = expectEqual(std::chrono::duration_cast<std::chrono::milliseconds>(earth_time::time_offset).count(), 0LL, "final reset offset") && ok;

    return ok;
}
