#include "test_manaclipper_arrival_event_2897.h"

#include "map/manaclipper_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "manaclipper arrival event 2897 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline xi.manaclipper.onZoneIn route → CSID select for dual-wire
// cross-check (slice 2897):
//
//   if route == dest.PURGONORGO_ISLE then 13 else 12
auto inlineArrivalEventID(const int32 route) -> int32
{
    if (route == manaclipperhelpers::kDestPurgonorgoIsle)
    {
        return 13;
    }
    return 12;
}

} // namespace

// Pure dual-wire expansion for manaclipperhelpers::ArrivalEventID
// (onZoneIn arrival CSID after route inject; slice 2897).
auto runManaclipperArrivalEvent2897SelfTests() -> bool
{
    using manaclipperhelpers::ArrivalEventID;
    using manaclipperhelpers::kArrivalEventPurgonorgoIsle;
    using manaclipperhelpers::kArrivalEventSunsetDocks;
    using manaclipperhelpers::kDestDhalmelRock;
    using manaclipperhelpers::kDestMaliyakaleyaReef;
    using manaclipperhelpers::kDestPurgonorgoIsle;
    using manaclipperhelpers::kDestSunsetDocks;

    bool ok = true;

    // Dest enum pins (match Go Dest / Lua dest table).
    ok = expect(kDestDhalmelRock == 0, "Dest DHALMEL_ROCK pin") && ok;
    ok = expect(kDestMaliyakaleyaReef == 1, "Dest MALIYAKALEYA_REEF pin") && ok;
    ok = expect(kDestPurgonorgoIsle == 2, "Dest PURGONORGO_ISLE pin") && ok;
    ok = expect(kDestSunsetDocks == 3, "Dest SUNSET_DOCKS pin") && ok;

    // Arrival CSID pins.
    ok = expect(kArrivalEventPurgonorgoIsle == 13, "arrival CSID Purgonorgo pin") && ok;
    ok = expect(kArrivalEventSunsetDocks == 12, "arrival CSID Sunset pin") && ok;

    // Residual 0942 pins + dual-wire table.
    const struct
    {
        int32       route;
        int32       want;
        const char* label;
    } cases[] = {
        { kDestPurgonorgoIsle, 13, "PURGONORGO_ISLE residual 0942" },
        { kDestDhalmelRock, 12, "DHALMEL_ROCK residual 0942" },
        { kDestMaliyakaleyaReef, 12, "MALIYAKALEYA_REEF residual 0942" },
        { kDestSunsetDocks, 12, "SUNSET_DOCKS residual 0942" },
        { 4, 12, "unknown route falls to 12" },
        { -1, 12, "negative route falls to 12" },
        { 99, 12, "large unknown route falls to 12" },
    };

    for (const auto& c : cases)
    {
        const int32 got     = ArrivalEventID(c.route);
        const int32 inlineV = inlineArrivalEventID(c.route);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineV, "ArrivalEventID dual-wire free == inline") && ok;
    }

    // Compose identity: free == 13 iff PURGONORGO else 12 for catalog Dest.
    const int32 catalog[] = {
        kDestDhalmelRock,
        kDestMaliyakaleyaReef,
        kDestPurgonorgoIsle,
        kDestSunsetDocks,
    };
    for (const int32 route : catalog)
    {
        const int32 got  = ArrivalEventID(route);
        const int32 want = (route == kDestPurgonorgoIsle) ? 13 : 12;
        ok = expect(got == want, "compose free == formula") && ok;
        ok = expect(got == inlineArrivalEventID(route), "compose free == inline") && ok;
    }

    return ok;
}
