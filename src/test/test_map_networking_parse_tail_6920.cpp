#include "test_map_networking_parse_tail_6920.h"

#include "map/map_networking_parse_tail.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking parse tail 6920 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingParseTail6920SelfTests() -> bool
{
    using mapnetworkinghelpers::AcknowledgementPlan;
    using mapnetworkingparsetailhelpers::MakePlan;

    bool ok = true;

    auto plan = MakePlan(0x1234, 7, 7, 0x001);
    ok        = expect(plan.clientPacketID == 0x1234 && plan.acknowledgement == AcknowledgementPlan::IncrementServerPacketID,
                 "exact acknowledgement increments") &&
         ok;

    plan = MakePlan(0x5678, 6, 7, 0x00A);
    ok   = expect(plan.clientPacketID == 0x5678 && plan.acknowledgement == AcknowledgementPlan::IgnoreLoginMismatch,
                "mismatched login ignores") &&
         ok;

    plan = MakePlan(0x9ABC, 6, 7, 0x001);
    ok   = expect(plan.clientPacketID == 0x9ABC && plan.acknowledgement == AcknowledgementPlan::ReplayCachedPacket,
                "mismatched ordinary replays") &&
         ok;

    return ok;
}
