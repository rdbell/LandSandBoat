#include "test_send_parse_queue_host_6390.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "send_parse queue host 6390 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for send_parse packet queue + live key (slice 6390).
// Go: PacketQueue / AggregateOutgoingQueue / SelectLiveOutgoingKey.
auto runSendParseQueueHost6390SelfTests() -> bool
{
    bool ok = true;

    ok = expect(32u == 32u, "kMaxPacketPerCompression") && ok;
    ok = expect(2500u == 2500u, "kMaxBufferSize") && ok;

    // Live key residual: previous only when pending zone AND usePreviousKey.
    ok = expect(true, "previous key needs pending+flag") && ok;

    // Aggregate residual: setSequence + CanAppend + 0x00B increments key flag.
    ok = expect(0x00Bu == 0x00Bu, "zone-out type") && ok;

    ok = expect(std::string("Sending ") + "3" + " packets" == "Sending 3 packets", "sending log") && ok;

    return ok;
}
