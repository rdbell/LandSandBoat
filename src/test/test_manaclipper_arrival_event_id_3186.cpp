#include "test_manaclipper_arrival_event_id_3186.h"

#include "map/manaclipper_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "manaclipper ArrivalEventID 3186 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline xi.manaclipper.onZoneIn route → CSID select for dual-wire
// cross-check (dedicated slice 3186; Lua literals 13/12):
//
//   if route == dest.PURGONORGO_ISLE then 13 else 12
auto inlineArrivalEventID3186(const int32 route) -> int32
{
    if (route == manaclipperhelpers::kDestPurgonorgoIsle)
    {
        return 13;
    }
    return 12;
}

// Compact dual-wire pin matching Go pinArrivalEventID3186 / C++ capacity:
//   if route == kDestPurgonorgoIsle:
//     return kArrivalEventPurgonorgoIsle
//   return kArrivalEventSunsetDocks
auto pinArrivalEventID3186(const int32 route) -> int32
{
    if (route == manaclipperhelpers::kDestPurgonorgoIsle)
    {
        return manaclipperhelpers::kArrivalEventPurgonorgoIsle;
    }
    return manaclipperhelpers::kArrivalEventSunsetDocks;
}

} // namespace

// Pure dual-wire expansion for manaclipperhelpers::ArrivalEventID
// (Lua onZoneIn arrival CSID inject form;
// OmegaXI internal/manaclipper; slice 3186).
//
// Coverage:
//   - free == inline == pin
//   - poles for all Dest values (0..3) + defensive unknowns
//   - Dest pins (0..3) and arrival CSID pins (13/12)
//   - residual 0942 / 2897 pins still hold
//   - sibling RemainingGameMinutes residual independence
auto runManaclipperArrivalEventID3186SelfTests() -> bool
{
    using manaclipperhelpers::ArrivalEventID;
    using manaclipperhelpers::kArrivalEventPurgonorgoIsle;
    using manaclipperhelpers::kArrivalEventSunsetDocks;
    using manaclipperhelpers::kDestDhalmelRock;
    using manaclipperhelpers::kDestMaliyakaleyaReef;
    using manaclipperhelpers::kDestPurgonorgoIsle;
    using manaclipperhelpers::kDestSunsetDocks;
    using manaclipperhelpers::kMinutesPerGameDay;
    using manaclipperhelpers::RemainingGameMinutes;

    bool ok = true;

    // Dest enum pins (match Go Dest / Lua dest table).
    ok = expect(kDestDhalmelRock == 0, "Dest DHALMEL_ROCK pin") && ok;
    ok = expect(kDestMaliyakaleyaReef == 1, "Dest MALIYAKALEYA_REEF pin") && ok;
    ok = expect(kDestPurgonorgoIsle == 2, "Dest PURGONORGO_ISLE pin") && ok;
    ok = expect(kDestSunsetDocks == 3, "Dest SUNSET_DOCKS pin") && ok;

    // Arrival CSID pins.
    ok = expect(kArrivalEventPurgonorgoIsle == 13, "arrival CSID Purgonorgo pin") && ok;
    ok = expect(kArrivalEventSunsetDocks == 12, "arrival CSID Sunset pin") && ok;

    // Residual 0942 / 2897 pins still hold under dual-wire.
    ok = expect(ArrivalEventID(kDestPurgonorgoIsle) == 13, "residual 0942: PURGONORGO → 13") && ok;
    ok = expect(ArrivalEventID(kDestDhalmelRock) == 12, "residual 0942: DHALMEL → 12") && ok;
    ok = expect(ArrivalEventID(kDestMaliyakaleyaReef) == 12, "residual 0942: MALIYAKALEYA → 12") && ok;
    ok = expect(ArrivalEventID(kDestSunsetDocks) == 12, "residual 0942: SUNSET → 12") && ok;
    ok = expect(ArrivalEventID(4) == 12, "residual 2897: unknown 4 → 12") && ok;
    ok = expect(ArrivalEventID(-1) == 12, "residual 2897: negative → 12") && ok;
    ok = expect(ArrivalEventID(99) == 12, "residual 2897: large unknown → 12") && ok;

    // --- Core poles: free == inline == pin (all Dest values + defensive) ---
    const struct
    {
        int32       route;
        int32       want;
        const char* label;
    } cases[] = {
        // Catalog Dest poles (all 0..3).
        { kDestDhalmelRock, 12, "Dest pole DHALMEL_ROCK → 12" },
        { kDestMaliyakaleyaReef, 12, "Dest pole MALIYAKALEYA_REEF → 12" },
        { kDestPurgonorgoIsle, 13, "Dest pole PURGONORGO_ISLE → 13" },
        { kDestSunsetDocks, 12, "Dest pole SUNSET_DOCKS → 12" },

        // Residual 0942 / 2897 re-pins in table.
        { kDestPurgonorgoIsle, 13, "residual 0942 PURGONORGO" },
        { kDestDhalmelRock, 12, "residual 0942 DHALMEL" },
        { kDestMaliyakaleyaReef, 12, "residual 0942 MALIYAKALEYA" },
        { kDestSunsetDocks, 12, "residual 0942 SUNSET" },
        { 4, 12, "residual 2897 unknown 4" },
        { -1, 12, "residual 2897 negative" },
        { 99, 12, "residual 2897 large unknown" },

        // Extra defensive poles.
        { 0, 12, "literal 0 DHALMEL → 12" },
        { 1, 12, "literal 1 MALIYAKALEYA → 12" },
        { 2, 13, "literal 2 PURGONORGO → 13" },
        { 3, 12, "literal 3 SUNSET → 12" },
        { 5, 12, "unknown 5 → 12" },
        { 100, 12, "unknown 100 → 12" },
        { -99, 12, "unknown -99 → 12" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = ArrivalEventID(c.route);
        const int32 inlineV = inlineArrivalEventID3186(c.route);
        const int32 pinV    = pinArrivalEventID3186(c.route);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineV, "ArrivalEventID free == inline") && ok;
        ok = expect(got == pinV, "ArrivalEventID free == pin") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ArrivalEventID(kDestPurgonorgoIsle) == pinArrivalEventID3186(kDestPurgonorgoIsle),
                "free == pin PURGONORGO residual") &&
         ok;
    ok = expect(ArrivalEventID(kDestSunsetDocks) == pinArrivalEventID3186(kDestSunsetDocks),
                "free == pin SUNSET residual") &&
         ok;
    ok = expect(ArrivalEventID(kDestDhalmelRock) == pinArrivalEventID3186(kDestDhalmelRock),
                "free == pin DHALMEL residual") &&
         ok;
    ok = expect(ArrivalEventID(99) == pinArrivalEventID3186(99),
                "free == pin unknown residual") &&
         ok;

    // Dense compose over representative route poles — free == inline == pin.
    const int32 routes[] = {
        kDestDhalmelRock,
        kDestMaliyakaleyaReef,
        kDestPurgonorgoIsle,
        kDestSunsetDocks,
        -5,
        -1,
        0,
        1,
        2,
        3,
        4,
        5,
        10,
        50,
        99,
        100,
    };
    for (const int32 route : routes)
    {
        const int32 got     = ArrivalEventID(route);
        const int32 inlineV = inlineArrivalEventID3186(route);
        const int32 pinV    = pinArrivalEventID3186(route);
        const int32 want    = (route == kDestPurgonorgoIsle) ? 13 : 12;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineV, "compose free == inline") && ok;
        ok = expect(got == pinV, "compose free == pin") && ok;
    }

    // Sibling RemainingGameMinutes residual still independent (not re-expanded
    // under 3186; residual/dedicated 3162 owns RemainingGameMinutes suite).
    ok = expect(RemainingGameMinutes(10, 50) == 40,
                "sibling residual: RemainingGameMinutes(10,50) want 40") &&
         ok;
    ok = expect(RemainingGameMinutes(1050, 10) == 400,
                "sibling residual: RemainingGameMinutes(1050,10) want 400") &&
         ok;
    ok = expect(kMinutesPerGameDay == 1440,
                "sibling residual: MinutesPerGameDay want 1440") &&
         ok;

    return ok;
}
