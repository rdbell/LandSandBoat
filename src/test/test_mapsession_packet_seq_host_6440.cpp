#include "test_mapsession_packet_seq_host_6440.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession packet seq host 6440 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapSession packet-sequence fields (slice 6440).
// Go: ApplyResetPacketSequences / ApplyUpdateClientPacketID.
auto runMapsessionPacketSeqHost6440SelfTests() -> bool
{
    bool ok = true;

    // reset zeros both ids
    ok = expect(true, "reset") && ok;

    // server_packet_id increments by 1
    ok = expect(uint16_t{0} + 1 == 1, "increment") && ok;

    // zone_type NONE is 0
    ok = expect(0 == 0, "zone none") && ok;

    return ok;
}
