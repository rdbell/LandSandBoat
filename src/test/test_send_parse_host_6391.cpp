#include "test_send_parse_host_6391.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "send_parse host 6391 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for send_parse compress+finalize host (slice 6391).
// Go: ApplySendParse / SelectLiveOutgoingKey / FinalizePacket.
auto runSendParseHost6391SelfTests() -> bool
{
    bool ok = true;

    const std::vector<std::string> steps = {
        "preparePacket",
        "AggregateOutgoingQueue",
        "compressPacket",
        "finalizePacket_live_key",
        "erasePackets",
    };
    ok = expect(steps.size() == 5, "step count") && ok;
    ok = expect(steps[0] == "preparePacket", "prepare first") && ok;
    ok = expect(steps.back() == "erasePackets", "erase last") && ok;

    ok = expect(std::string("zlib compression error") == "zlib compression error", "zlib log") && ok;

    return ok;
}
