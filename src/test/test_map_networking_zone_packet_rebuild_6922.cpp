#include "test_map_networking_zone_packet_rebuild_6922.h"

#include "map/map_networking_zone_packet_rebuild.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking zone packet rebuild 6922 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapNetworkingZonePacketRebuild6922SelfTests() -> bool
{
    using mapnetworkingzonepacketrebuildhelpers::MakeCompletionPlan;
    using mapnetworkingzonepacketrebuildhelpers::MakePacketPlan;

    bool ok = true;

    const auto packet = MakePacketPlan(0xFFFF);
    ok = expect(packet.packetSequence == 0xFFFF && packet.usePreviousKey,
                "rebuild uses current sequence and previous key") &&
         ok;

    auto completion = MakeCompletionPlan(0xFFFF, true);
    ok = expect(!completion.clearOutput && completion.incrementPacketsSent && completion.nextServerPacketID == 0,
                "successful rebuild counts sent packet and wraps sequence") &&
         ok;

    completion = MakeCompletionPlan(0xFFFF, false);
    ok = expect(completion.clearOutput && !completion.incrementPacketsSent && completion.nextServerPacketID == 0,
                "failed compression clears output but still wraps sequence") &&
         ok;

    return ok;
}
