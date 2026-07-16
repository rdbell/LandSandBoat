#include "test_transport_schedule_2646.h"

#include <iostream>

#include "map/transport.h"

auto runTransportSchedule2646SelfTests() -> bool
{
    bool valid = false;
    const TransportScheduleInput input{
        .timeOffset     = 11,
        .timeInterval   = 100,
        .timeWaiting    = 20,
        .timeAnimArrive = 10,
        .timeAnimDepart = 6,
    };
    const auto town   = transporthelpers::BuildTownSchedule(input, valid);
    const auto voyage = transporthelpers::BuildVoyageSchedule(input);

    bool ok = valid &&
              town.timeOffset == xi::vanadiel_clock::minutes(11) &&
              town.timeArriveDock == xi::vanadiel_clock::minutes(10) &&
              town.timeDepartDock == xi::vanadiel_clock::minutes(30) &&
              town.timeVoyageStart == xi::vanadiel_clock::minutes(35) &&
              voyage.timeVoyageStart == xi::vanadiel_clock::minutes(36);

    const auto arrivalBoundary = transporthelpers::BuildTownSchedule({ 0, 14, 0, 9, 6 }, valid);
    ok                         = !valid && arrivalBoundary.timeVoyageStart == xi::vanadiel_clock::minutes(14) && ok;

    const auto intervalBelow = transporthelpers::BuildTownSchedule({ 0, 14, 0, 10, 6 }, valid);
    ok                       = !valid && intervalBelow.timeVoyageStart == xi::vanadiel_clock::minutes(15) && ok;

    const auto intervalBoundary = transporthelpers::BuildTownSchedule({ 0, 15, 0, 10, 6 }, valid);
    ok                          = valid && intervalBoundary.timeInterval == intervalBoundary.timeVoyageStart && ok;

    const auto zeroDeparture = transporthelpers::BuildTownSchedule({ 0, 100, 0, 10, 0 }, valid);
    ok                       = !valid && zeroDeparture.timeVoyageStart == xi::vanadiel_clock::minutes(10) + xi::vanadiel_clock::minutes(UINT32_MAX) && ok;

    if (!ok)
    {
        std::cerr << "transport schedule self-test failed\n";
    }
    return ok;
}
