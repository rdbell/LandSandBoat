#include "test_map_networking_small_packet_disposition_6919.h"

#include "map/map_networking_small_packet.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking small packet disposition 6919 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingSmallPacketDisposition6919SelfTests() -> bool
{
    using mapnetworkingsmallpackethelpers::Disposition;
    using mapnetworkingsmallpackethelpers::PlanDisposition;

    bool ok = true;

    ok = expect(PlanDisposition(10, 10, 10, true, true, false) == Disposition::SkipSequence,
                "sequence rejection precedes login policy") &&
         ok;
    ok = expect(PlanDisposition(11, 10, 11, true, false, false) == Disposition::Dispatch,
                "zoned in-window packet dispatches") &&
         ok;
    ok = expect(PlanDisposition(11, 10, 11, false, true, true) == Disposition::Dispatch,
                "login dispatches before zone attachment") &&
         ok;
    ok = expect(PlanDisposition(11, 10, 11, false, false, true) == Disposition::SkipPendingZone,
                "pending zone skips unexpected packet") &&
         ok;
    ok = expect(PlanDisposition(11, 10, 11, false, false, false) == Disposition::WarnAndSkip,
                "unzoned packet warns and skips") &&
         ok;

    return ok;
}
