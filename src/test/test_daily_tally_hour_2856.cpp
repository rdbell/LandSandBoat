#include "test_daily_tally_hour_2856.h"

#include "world/daily_tally.h"
#include "world/daily_tally_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "daily tally hour 2856 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline historical time_server formula for dual-wire cross-checks.
auto inlineShouldRun(const bool hourlyTickDue, const int jstHour) -> bool
{
    return hourlyTickDue && jstHour == 0;
}

auto inlineIsMidnight(const int jstHour) -> bool
{
    return jstHour == 0;
}

} // namespace

// Pure dual-wire expansion for dailytallyhelpers::ShouldRunDailyTallyOnHourlyTick
// / IsJSTDailyTickHour and dailytally free-function dual-wires (slice 2856).
// Schedule gate: hourlyTickDue && jstHour == 0.
auto runDailyTallyHour2856SelfTests() -> bool
{
    using dailytallyhelpers::IsJSTDailyTickHour;
    using dailytallyhelpers::ShouldRunDailyTallyOnHourlyTick;

    bool ok = true;

    // IsJSTDailyTickHour: only hour 0.
    ok = expect(IsJSTDailyTickHour(0), "midnight is daily hour") && ok;
    ok = expect(!IsJSTDailyTickHour(1), "hour 1 is not daily") && ok;
    ok = expect(!IsJSTDailyTickHour(12), "hour 12 is not daily") && ok;
    ok = expect(!IsJSTDailyTickHour(23), "hour 23 is not daily") && ok;
    ok = expect(!IsJSTDailyTickHour(-1), "negative hour is not daily") && ok;

    // ShouldRunDailyTallyOnHourlyTick table.
    const struct
    {
        bool        hourlyTickDue;
        int         jstHour;
        bool        want;
        const char* label;
    } cases[] = {
        { true, 0, true, "due midnight" },
        { true, 1, false, "due hour 1" },
        { true, 12, false, "due noon" },
        { true, 23, false, "due hour 23" },
        { false, 0, false, "not due midnight" },
        { false, 1, false, "not due hour 1" },
        { false, 12, false, "not due noon" },
        { true, -1, false, "due negative hour" },
        { false, -1, false, "not due negative hour" },
    };

    for (const auto& c : cases)
    {
        const bool got       = ShouldRunDailyTallyOnHourlyTick(c.hourlyTickDue, c.jstHour);
        const bool inlineGot = inlineShouldRun(c.hourlyTickDue, c.jstHour);
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
        ok                   = expect(IsJSTDailyTickHour(c.jstHour) == inlineIsMidnight(c.jstHour), "hour pin dual-wire") && ok;
        // Composition: ShouldRun == hourlyTickDue && IsJSTDailyTickHour.
        ok = expect(got == (c.hourlyTickDue && IsJSTDailyTickHour(c.jstHour)), "compose due && hour") && ok;
    }

    // Host free functions dual-wire pure helpers.
    ok = expect(dailytally::IsJSTDailyTickHour(0) == IsJSTDailyTickHour(0), "host IsJST midnight") && ok;
    ok = expect(dailytally::IsJSTDailyTickHour(1) == IsJSTDailyTickHour(1), "host IsJST hour 1") && ok;
    ok = expect(dailytally::ShouldRunDailyTallyOnHourlyTick(true, 0) == ShouldRunDailyTallyOnHourlyTick(true, 0),
                "host ShouldRun due midnight") &&
         ok;
    ok = expect(dailytally::ShouldRunDailyTallyOnHourlyTick(true, 1) == ShouldRunDailyTallyOnHourlyTick(true, 1),
                "host ShouldRun due hour 1") &&
         ok;
    ok = expect(dailytally::ShouldRunDailyTallyOnHourlyTick(false, 0) == ShouldRunDailyTallyOnHourlyTick(false, 0),
                "host ShouldRun not due midnight") &&
         ok;

    // Production dispatch path pins: only due+midnight runs daily tally.
    ok = expect(dailytally::ShouldRunDailyTallyOnHourlyTick(true, 0), "production midnight path") && ok;
    ok = expect(!dailytally::ShouldRunDailyTallyOnHourlyTick(true, 13), "production ordinary hour path") && ok;
    ok = expect(!dailytally::ShouldRunDailyTallyOnHourlyTick(false, 0), "production not-due midnight") && ok;

    return ok;
}
