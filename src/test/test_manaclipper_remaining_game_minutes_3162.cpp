#include "test_manaclipper_remaining_game_minutes_3162.h"

#include "map/manaclipper_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "manaclipper RemainingGameMinutes 3162 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline xi.manaclipper.timekeeperOnTrigger game-minute countdown for
// dual-wire cross-check (dedicated slice 3162; Lua literal 1440):
//
//   gameMins = endTime - currentTime
//   if endTime < currentTime:
//     gameMins = 1440 + endTime - currentTime
auto inlineRemainingGameMinutes3162(const int32 currentTime, const int32 endTime) -> int32
{
    int32 gameMins = endTime - currentTime;
    if (endTime < currentTime)
    {
        gameMins = 1440 + endTime - currentTime;
    }
    return gameMins;
}

// Compact dual-wire pin matching Go pinRemainingGameMinutes3162 / C++ capacity:
//   if endTime < currentTime:
//     return kMinutesPerGameDay + endTime - currentTime
//   return endTime - currentTime
auto pinRemainingGameMinutes3162(const int32 currentTime, const int32 endTime) -> int32
{
    if (endTime < currentTime)
    {
        return manaclipperhelpers::kMinutesPerGameDay + endTime - currentTime;
    }
    return endTime - currentTime;
}

} // namespace

// Pure dual-wire expansion for manaclipperhelpers::RemainingGameMinutes
// (Lua timekeeperOnTrigger countdown inject form;
// OmegaXI internal/manaclipper; slice 3162).
//
// Coverage:
//   - free == inline == pin
//   - wrap and non-wrap poles
//   - kMinutesPerGameDay == 1440
//   - residual 0942 / 2887 pins still hold
//   - sibling ArrivalEventID residual independence
auto runManaclipperRemainingGameMinutes3162SelfTests() -> bool
{
    using manaclipperhelpers::ArrivalEventID;
    using manaclipperhelpers::kArrivalEventPurgonorgoIsle;
    using manaclipperhelpers::kArrivalEventSunsetDocks;
    using manaclipperhelpers::kDestPurgonorgoIsle;
    using manaclipperhelpers::kDestSunsetDocks;
    using manaclipperhelpers::kMinutesPerGameDay;
    using manaclipperhelpers::RemainingGameMinutes;

    bool ok = true;

    // Constant pin (match Go MinutesPerGameDay / Lua literal 1440).
    ok = expect(kMinutesPerGameDay == 1440, "MinutesPerGameDay pin") && ok;

    // Residual 0942 / 2887 pins still hold under dual-wire.
    ok = expect(RemainingGameMinutes(10, 50) == 40, "residual 0942: same-day 10→50") && ok;
    ok = expect(RemainingGameMinutes(1050, 10) == 400, "residual 0942: wrap 1050→10") && ok;
    ok = expect(RemainingGameMinutes(50, 50) == 0, "residual 0942: equal 50→50") && ok;
    ok = expect(RemainingGameMinutes(0, 0) == 0, "residual 2887: midnight equal") && ok;
    ok = expect(RemainingGameMinutes(1439, 0) == 1, "residual 2887: one minute wrap") && ok;
    ok = expect(RemainingGameMinutes(1380, 275) == 335, "residual 2887: barge-style wrap") && ok;

    // --- Core poles: free == inline == pin (wrap and non-wrap) ---
    const struct
    {
        int32       currentTime;
        int32       endTime;
        int32       want;
        const char* label;
    } cases[] = {
        // Non-wrap poles (endTime >= currentTime).
        { 10, 50, 40, "non-wrap residual 0942 same-day" },
        { 50, 50, 0, "non-wrap equal residual 0942" },
        { 0, 0, 0, "non-wrap midnight equal" },
        { 0, 1, 1, "non-wrap one minute same day" },
        { 900, 960, 60, "non-wrap interior same-day hour" },
        { 0, 1440, 1440, "non-wrap full day (end==MinutesPerGameDay)" },
        { 720, 720, 0, "non-wrap midday equal" },
        { 0, 720, 720, "non-wrap half day" },
        { 100, 200, 100, "non-wrap 100-minute residual" },
        { 1439, 1439, 0, "non-wrap last minute equal" },
        { 1439, 1440, 1, "non-wrap last-to-end same day" },

        // Wrap poles (endTime < currentTime → +1440).
        { 1050, 10, 400, "wrap residual 0942 (1440+10-1050)" },
        { 1439, 0, 1, "wrap one minute (1439→0)" },
        { 1439, 10, 11, "wrap near day end" },
        { 1380, 275, 335, "wrap barge-style pin" },
        { 721, 720, 1439, "wrap one past equal almost full day" },
        { 1, 0, 1439, "wrap one past midnight" },
        { 1440, 0, 0, "wrap current==MinutesPerGameDay end 0" },
        { 1440, 10, 10, "wrap current==MinutesPerGameDay end 10" },
        { 720, 0, 720, "wrap midday → midnight" },
        { 1430, 5, 15, "wrap late evening short residual" },

        // Residual 2887 / 0942 re-pins in table.
        { 10, 50, 40, "residual 2887 same-day" },
        { 1050, 10, 400, "residual 2887 wrap" },
        { 50, 50, 0, "residual 0942 equal" },
        { 0, 1440, 1440, "residual 2887 full day" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = RemainingGameMinutes(c.currentTime, c.endTime);
        const int32 inlineV = inlineRemainingGameMinutes3162(c.currentTime, c.endTime);
        const int32 pinV    = pinRemainingGameMinutes3162(c.currentTime, c.endTime);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineV, "RemainingGameMinutes free == inline") && ok;
        ok = expect(got == pinV, "RemainingGameMinutes free == pin") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(RemainingGameMinutes(10, 50) == pinRemainingGameMinutes3162(10, 50),
                "free == pin same-day residual") &&
         ok;
    ok = expect(RemainingGameMinutes(1050, 10) == pinRemainingGameMinutes3162(1050, 10),
                "free == pin wrap residual") &&
         ok;
    ok = expect(RemainingGameMinutes(50, 50) == pinRemainingGameMinutes3162(50, 50),
                "free == pin equal residual") &&
         ok;
    ok = expect(RemainingGameMinutes(721, 720) == pinRemainingGameMinutes3162(721, 720),
                "free == pin near-equal wrap") &&
         ok;

    // Dense compose over representative poles — free == inline == pin.
    for (int32 current = 0; current <= 1440; current += 30)
    {
        for (int32 end = 0; end <= 1440; end += 30)
        {
            const int32 got     = RemainingGameMinutes(current, end);
            const int32 inlineV = inlineRemainingGameMinutes3162(current, end);
            const int32 pinV    = pinRemainingGameMinutes3162(current, end);
            ok = expect(got == inlineV, "compose free == inline") && ok;
            ok = expect(got == pinV, "compose free == pin") && ok;
            // Result is always in [0, 1440] for inputs in that domain
            // (wrap of current=1440,end=0 yields 0).
            ok = expect(got >= 0 && got <= kMinutesPerGameDay, "result in [0,1440]") && ok;
        }
    }

    // Sibling ArrivalEventID residual still independent (not re-expanded under
    // 3162; residual 2897 owns dedicated ArrivalEventID suite).
    ok = expect(ArrivalEventID(kDestPurgonorgoIsle) == kArrivalEventPurgonorgoIsle,
                "sibling residual: ArrivalEventID(PURGONORGO) want 13") &&
         ok;
    ok = expect(ArrivalEventID(kDestSunsetDocks) == kArrivalEventSunsetDocks,
                "sibling residual: ArrivalEventID(SUNSET) want 12") &&
         ok;

    return ok;
}
