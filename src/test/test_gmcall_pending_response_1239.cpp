#include "test_gmcall_pending_response_1239.h"

#include "map/gmcall_container.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "GM-call pending response 1239 self-test failed: " << label << '\n';
    }
    return condition;
}

auto testEmptyResponseProducesNoPackets() -> bool
{
    return expect(gmcall::detail::BuildPendingResponsePackets(7, "").empty(), "empty response");
}

auto testPacketBoundariesAndMetadata() -> bool
{
    const std::string response(489, 'x');
    const auto        packets = gmcall::detail::BuildPendingResponsePackets(0x12345678, response);

    if (!expect(packets.size() == 3, "489-byte response packet count"))
    {
        return false;
    }

    bool ok = true;
    ok      = expect(packets[0].callId == 0x12345678 && packets[0].seqId == 1 && packets[0].pktNum == 1,
                "first packet metadata") &&
         ok;
    ok = expect(packets[1].seqId == 2 && packets[1].pktNum == 2, "middle packet metadata") && ok;
    ok = expect(packets[2].seqId == 3 && packets[2].pktNum == 0, "final packet metadata") && ok;
    ok = expect(packets[0].message.size() == 244 && packets[1].message.size() == 244 && packets[2].message.size() == 1,
                "packet boundary sizes") &&
         ok;
    return ok;
}

auto testExactBoundaryIsFinal() -> bool
{
    const auto packets = gmcall::detail::BuildPendingResponsePackets(9, std::string(244, 'a'));
    return expect(packets.size() == 1, "exact boundary packet count") &&
           expect(packets[0].seqId == 1 && packets[0].pktNum == 0 && packets[0].message.size() == 244,
                  "exact boundary final metadata");
}

auto testBinarySafeByteChunking() -> bool
{
    std::string response(245, 'b');
    response[100] = '\0';
    response[243] = static_cast<char>(0xFF);

    const auto packets = gmcall::detail::BuildPendingResponsePackets(11, response);
    return expect(packets.size() == 2, "binary response packet count") &&
           expect(packets[0].message == response.substr(0, 244), "binary first chunk") &&
           expect(packets[1].message == response.substr(244), "binary second chunk");
}

auto testApplicationMaximumResponse() -> bool
{
    const auto packets = gmcall::detail::BuildPendingResponsePackets(12, std::string(1024, 'z'));
    return expect(packets.size() == 5, "1024-byte application maximum packet count") &&
           expect(packets.back().seqId == 5 && packets.back().pktNum == 0 && packets.back().message.size() == 48,
                  "1024-byte application maximum final packet");
}

auto testChunkerDoesNotApplyApplicationLimit() -> bool
{
    const auto packets = gmcall::detail::BuildPendingResponsePackets(13, std::string(1025, 'q'));
    return expect(packets.size() == 5, "1025-byte direct response packet count") &&
           expect(packets.back().seqId == 5 && packets.back().pktNum == 0 && packets.back().message.size() == 49,
                  "1025-byte direct response final packet");
}

} // namespace

auto runGMCallPendingResponse1239SelfTests() -> bool
{
    bool ok = true;
    ok      = testEmptyResponseProducesNoPackets() && ok;
    ok      = testPacketBoundariesAndMetadata() && ok;
    ok      = testExactBoundaryIsFinal() && ok;
    ok      = testBinarySafeByteChunking() && ok;
    ok      = testApplicationMaximumResponse() && ok;
    ok      = testChunkerDoesNotApplyApplicationLimit() && ok;
    return ok;
}
