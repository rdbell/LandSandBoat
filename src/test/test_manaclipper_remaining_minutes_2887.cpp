#include "test_manaclipper_remaining_minutes_2887.h"

#include "map/manaclipper_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "manaclipper remaining minutes 2887 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline xi.manaclipper.timekeeperOnTrigger game-minute countdown for
// dual-wire cross-check (slice 2887):
//
//   gameMins = endTime - currentTime
//   if endTime < currentTime:
//     gameMins = 1440 + endTime - currentTime
auto inlineRemainingGameMinutes(const int32 currentTime, const int32 endTime) -> int32
{
    int32 gameMins = endTime - currentTime;
    if (endTime < currentTime)
    {
        gameMins = 1440 + endTime - currentTime;
    }
    return gameMins;
}

} // namespace

// Pure dual-wire expansion for manaclipperhelpers::RemainingGameMinutes
// (timekeeperOnTrigger countdown after currentTime / endTime inject; slice 2887).
auto runManaclipperRemainingMinutes2887SelfTests() -> bool
{
    using manaclipperhelpers::kMinutesPerGameDay;
    using manaclipperhelpers::RemainingGameMinutes;

    bool ok = true;

    // Constant pin (match Go MinutesPerGameDay / Lua literal 1440).
    ok = expect(kMinutesPerGameDay == 1440, "MinutesPerGameDay pin") && ok;

    // Residual 0942 pins + dual-wire table.
    const struct
    {
        int32       currentTime;
        int32       endTime;
        int32       want;
        const char* label;
    } cases[] = {
        { 10, 50, 40, "same-day residual 0942" },
        { 1050, 10, 400, "wrap residual 0942 (1440+10-1050)" },
        { 50, 50, 0, "equal residual 0942" },
        { 0, 0, 0, "midnight equal" },
        { 0, 1, 1, "one minute same day" },
        { 1439, 0, 1, "one minute wrap (1439→0)" },
        { 1439, 10, 11, "wrap near day end" },
        { 1380, 275, 335, "barge-style wrap pin (cross-package)" },
        { 900, 960, 60, "interior same-day hour" },
        { 0, 1440, 1440, "full day same-day (end==MinutesPerGameDay)" },
        { 720, 720, 0, "midday equal" },
        { 721, 720, 1439, "one past equal wraps almost full day" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = RemainingGameMinutes(c.currentTime, c.endTime);
        const int32 inlineV = inlineRemainingGameMinutes(c.currentTime, c.endTime);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineV, "RemainingGameMinutes dual-wire free == inline") && ok;
    }

    // Compose identity: free == end-current when end>=current, else 1440+end-current.
    for (int32 current = 0; current <= 1440; current += 15)
    {
        for (int32 end = 0; end <= 1440; end += 15)
        {
            const int32 got = RemainingGameMinutes(current, end);
            int32       want = end - current;
            if (end < current)
            {
                want = kMinutesPerGameDay + end - current;
            }
            ok = expect(got == want, "compose free == formula") && ok;
            ok = expect(got == inlineRemainingGameMinutes(current, end), "compose free == inline") && ok;
            // Result is always in [0, 1440] for inputs in that domain
            // (wrap of current=1440,end=0 yields 0).
            ok = expect(got >= 0 && got <= kMinutesPerGameDay, "result in [0,1440]") && ok;
        }
    }

    return ok;
}
