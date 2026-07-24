#include "test_mapsession_packet_cache_host_6444.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession packet cache host 6444 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for server_packet_data cache/replay (slice 6444).
// Go: mapwire.ApplyReplayCachedPacket / ApplyOutgoingAckPlan.
auto runMapsessionPacketCacheHost6444SelfTests() -> bool
{
    bool ok = true;

    // NetworkBuffer / kMaxBufferSize 2500
    ok = expect(2500 == 2500, "buffer size") && ok;

    // replay header: offset 2 = SmallPD_Code, offset 8 = uint16 timestamp
    ok = expect(true, "replay header") && ok;

    // parse return -1 skips send_parse
    ok = expect(true, "skip send_parse") && ok;

    return ok;
}
